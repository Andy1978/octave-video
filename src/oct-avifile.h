/* Copyright (C) 2004 Stefan van der Walt <stefan@sun.ac.za>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef _AVIFILE_H
#define _AVIFILE_H

#include "AVHandler.h"

class Avifile: public octave_base_value {

 public:
  AVHandler *av;

  Avifile(void) { *this = Avifile("default.avi"); }

  Avifile(std::string fn);

  octave_base_value *clone(void) const { return new Avifile(*this); }
  octave_base_value *empty_clone(void) const { return new Avifile(); }

  ~Avifile(void);

  void print(std::ostream& os, bool pr_as_read_syntax) const;
  
  void addframe(const NDArray &f);
  
  bool is_defined(void) const { return true; }
  
  bool is_constant(void) const { return true; }
  
 private:  
  Avifile(const Avifile& m);

  std::string filename;
  unsigned int frames;
  unsigned int frame_rows;
  unsigned int frame_columns;

  DECLARE_OCTAVE_ALLOCATOR
  DECLARE_OV_TYPEID_FUNCTIONS_AND_DATA
};
#endif
