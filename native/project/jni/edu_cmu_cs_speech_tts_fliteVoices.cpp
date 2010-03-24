/*************************************************************************/
/*                                                                       */
/*                  Language Technologies Institute                      */
/*                     Carnegie Mellon University                        */
/*                         Copyright (c) 2010                            */
/*                        All Rights Reserved.                           */
/*                                                                       */
/*  Permission is hereby granted, free of charge, to use and distribute  */
/*  this software and its documentation without restriction, including   */
/*  without limitation the rights to use, copy, modify, merge, publish,  */
/*  distribute, sublicense, and/or sell copies of this work, and to      */
/*  permit persons to whom this work is furnished to do so, subject to   */
/*  the following conditions:                                            */
/*   1. The code must retain the above copyright notice, this list of    */
/*      conditions and the following disclaimer.                         */
/*   2. Any modifications must be clearly marked as such.                */
/*   3. Original authors' names are not deleted.                         */
/*   4. The authors' names are not used to endorse or promote products   */
/*      derived from this software without specific prior written        */
/*      permission.                                                      */
/*                                                                       */
/*  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         */
/*  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      */
/*  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   */
/*  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      */
/*  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    */
/*  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   */
/*  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          */
/*  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       */
/*  THIS SOFTWARE.                                                       */
/*                                                                       */
/*************************************************************************/
/*             Author:  Alok Parlikar (aup@cs.cmu.edu)                   */
/*               Date:  March 2010                                       */
/*************************************************************************/
/*                                                                       */
/*  Library classes to manage available flite voices                     */
/*                                                                       */
/*************************************************************************/

#include <vector>
using namespace std;

#include "edu_cmu_cs_speech_tts_fliteVoices.hh"

namespace FliteEngine {
  
  Voice::Voice(String flang, String fcountry, String fvar, 
	       t_voice_register_function freg, 
	       t_voice_unregister_function funreg)
  {
    language = flang;
    country = fcountry;
    variant = fvar;
    fliteVoice = NULL;
    regfunc = freg;
    unregfunc = funreg;
  }
  
  Voice::~Voice()
  {
    unregisterVoice();
  }

  bool Voice::isSameLocaleAs(String flang, String fcountry, String fvar)
  {
    if( (language == flang) &&
	(country == fcountry) &&
	(variant == fvar) )
      return true;
    else
      return false;
  }

  cst_voice* Voice::registerVoice()
  {
    fliteVoice = regfunc(voxdir_path);
    return fliteVoice;
  }

  void Voice::unregisterVoice()
  {
    if(fliteVoice == NULL) return; // Voice not registered
    unregfunc(fliteVoice);
    fliteVoice = NULL;
  }
  
  cst_voice* Voice::getFliteVoice()
  {
    return fliteVoice;
  }

  Voices::Voices(int fregistrationMode)
  {
    rMode = fregistrationMode;
    currentVoice = NULL;
  }

  Voices::~Voices()
  {
    /* Items in the voiceList vector will be destroyed automatically
       and unregistered in their destructors,
       so we don't really have to do anything here
    */
  }

  void Voices::addVoice(String flang, String fcountry, String fvar, 
			t_voice_register_function freg,
			t_voice_unregister_function funreg)
  {
    Voice v = Voice(flang, fcountry, fvar, freg, funreg);
    
    /* We must register this voice if the registration mode
       so dictates.
    */

    if(rMode == ALL_VOICES_REGISTERED)
      v.registerVoice();
    voiceList.push_back(v);

  }

  bool Voices::isLocaleAvailable(String flang, String fcountry, String fvar)
  {
    vector<Voice>::iterator it;
    for(it=voiceList.begin(); it<voiceList.end();it++)
      if(it->isSameLocaleAs(flang, fcountry, fvar))
	{
	  return true;
	}
    return false;
  }

  cst_voice* Voices::getFliteVoiceForLocale(String flang, 
					    String fcountry, String fvar)
  {
    vector<Voice>::iterator it;  
    for(it=voiceList.begin(); it<voiceList.end();it++)
      if(it->isSameLocaleAs(flang, fcountry, fvar))
	{
	  if(rMode == ALL_VOICES_REGISTERED)
	    {
	      currentVoice = it;
	      return currentVoice->fliteVoice;
	    }
	  else
	    {
	      /* Only one voice can be registered.
		 Check that the one that the user wants
		 isn't already the current voice.
		 Otherwise, unregister current one
		 and then register and set the requested one
	      */
	      if(it == currentVoice)
		{
		  return currentVoice->fliteVoice;
		}
	      else
		{
		  currentVoice->unregisterVoice();
		  currentVoice = it;
		  return currentVoice->registerVoice();
		}

	}
    currentVoice = NULL; // Requested voice not available!
    return currentVoice;
  }
}
