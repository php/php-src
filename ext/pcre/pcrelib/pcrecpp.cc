// Copyright (c) 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Sanjay Ghemawat

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>      /* for SHRT_MIN, USHRT_MAX, etc */
#include <assert.h>
#include <errno.h>
#include <string>
#include <algorithm>
#include "config.h"
// We need this to compile the proper dll on windows/msys.  This is copied
// from pcre_internal.h.  It would probably be better just to include that.
#define PCRE_DEFINITION  /* Win32 __declspec(export) trigger for .dll */
#include "pcre.h"
#include "pcre_stringpiece.h"
#include "pcrecpp.h"


namespace pcrecpp {

// Maximum number of args we can set
static const int kMaxArgs = 16;
static const int kVecSize = (1 + kMaxArgs) * 3;  // results + PCRE workspace

// Special object that stands-in for no argument
Arg no_arg((void*)NULL);

// If a regular expression has no error, its error_ field points here
static const string empty_string;

// If the user doesn't ask for any options, we just use this one
static RE_Options default_options;

void RE::Init(const char* pat, const RE_Options* options) {
  pattern_ = pat;
  if (options == NULL) {
    options_ = default_options;
  } else {
    options_ = *options;
  }
  error_ = &empty_string;
  re_full_ = NULL;
  re_partial_ = NULL;

  re_partial_ = Compile(UNANCHORED);
  if (re_partial_ != NULL) {
    // Check for complicated patterns.  The following change is
    // conservative in that it may treat some "simple" patterns
    // as "complex" (e.g., if the vertical bar is in a character
    // class or is escaped).  But it seems good enough.
    if (strchr(pat, '|') == NULL) {
      // Simple pattern: we can use position-based checks to perform
      // fully anchored matches
      re_full_ = re_partial_;
    } else {
      // We need a special pattern for anchored matches
      re_full_ = Compile(ANCHOR_BOTH);
    }
  }
}

RE::~RE() {
  if (re_full_ != NULL && re_full_ != re_partial_) (*pcre_free)(re_full_);
  if (re_partial_ != NULL)                         (*pcre_free)(re_partial_);
  if (error_ != &empty_string)                     delete error_;
}

pcre* RE::Compile(Anchor anchor) {
  // First, convert RE_Options into pcre options
  int pcre_options = 0;
  pcre_options = options_.all_options();

  // Special treatment for anchoring.  This is needed because at
  // runtime pcre only provides an option for anchoring at the
  // beginning of a string (unless you use offset).
  //
  // There are three types of anchoring we want:
  //    UNANCHORED      Compile the original pattern, and use
  //                    a pcre unanchored match.
  //    ANCHOR_START    Compile the original pattern, and use
  //                    a pcre anchored match.
  //    ANCHOR_BOTH     Tack a "\z" to the end of the original pattern
  //                    and use a pcre anchored match.

  const char* compile_error;
  int eoffset;
  pcre* re;
  if (anchor != ANCHOR_BOTH) {
    re = pcre_compile(pattern_.c_str(), pcre_options,
                      &compile_error, &eoffset, NULL);
  } else {
    // Tack a '\z' at the end of RE.  Parenthesize it first so that
    // the '\z' applies to all top-level alternatives in the regexp.
    string wrapped = "(?:";  // A non-counting grouping operator
    wrapped += pattern_;
    wrapped += ")\\z";
    re = pcre_compile(wrapped.c_str(), pcre_options,
                      &compile_error, &eoffset, NULL);
  }
  if (re == NULL) {
    if (error_ == &empty_string) error_ = new string(compile_error);
  }
  return re;
}

/***** Matching interfaces *****/

bool RE::FullMatch(const StringPiece& text,
                   const Arg& ptr1,
                   const Arg& ptr2,
                   const Arg& ptr3,
                   const Arg& ptr4,
                   const Arg& ptr5,
                   const Arg& ptr6,
                   const Arg& ptr7,
                   const Arg& ptr8,
                   const Arg& ptr9,
                   const Arg& ptr10,
                   const Arg& ptr11,
                   const Arg& ptr12,
                   const Arg& ptr13,
                   const Arg& ptr14,
                   const Arg& ptr15,
                   const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  return DoMatchImpl(text, ANCHOR_BOTH, &consumed, args, n, vec, kVecSize);
}

bool RE::PartialMatch(const StringPiece& text,
                      const Arg& ptr1,
                      const Arg& ptr2,
                      const Arg& ptr3,
                      const Arg& ptr4,
                      const Arg& ptr5,
                      const Arg& ptr6,
                      const Arg& ptr7,
                      const Arg& ptr8,
                      const Arg& ptr9,
                      const Arg& ptr10,
                      const Arg& ptr11,
                      const Arg& ptr12,
                      const Arg& ptr13,
                      const Arg& ptr14,
                      const Arg& ptr15,
                      const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  return DoMatchImpl(text, UNANCHORED, &consumed, args, n, vec, kVecSize);
}

bool RE::Consume(StringPiece* input,
                 const Arg& ptr1,
                 const Arg& ptr2,
                 const Arg& ptr3,
                 const Arg& ptr4,
                 const Arg& ptr5,
                 const Arg& ptr6,
                 const Arg& ptr7,
                 const Arg& ptr8,
                 const Arg& ptr9,
                 const Arg& ptr10,
                 const Arg& ptr11,
                 const Arg& ptr12,
                 const Arg& ptr13,
                 const Arg& ptr14,
                 const Arg& ptr15,
                 const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  if (DoMatchImpl(*input, ANCHOR_START, &consumed,
                  args, n, vec, kVecSize)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RE::FindAndConsume(StringPiece* input,
                        const Arg& ptr1,
                        const Arg& ptr2,
                        const Arg& ptr3,
                        const Arg& ptr4,
                        const Arg& ptr5,
                        const Arg& ptr6,
                        const Arg& ptr7,
                        const Arg& ptr8,
                        const Arg& ptr9,
                        const Arg& ptr10,
                        const Arg& ptr11,
                        const Arg& ptr12,
                        const Arg& ptr13,
                        const Arg& ptr14,
                        const Arg& ptr15,
                        const Arg& ptr16) const {
  const Arg* args[kMaxArgs];
  int n = 0;
  if (&ptr1  == &no_arg) goto done; args[n++] = &ptr1;
  if (&ptr2  == &no_arg) goto done; args[n++] = &ptr2;
  if (&ptr3  == &no_arg) goto done; args[n++] = &ptr3;
  if (&ptr4  == &no_arg) goto done; args[n++] = &ptr4;
  if (&ptr5  == &no_arg) goto done; args[n++] = &ptr5;
  if (&ptr6  == &no_arg) goto done; args[n++] = &ptr6;
  if (&ptr7  == &no_arg) goto done; args[n++] = &ptr7;
  if (&ptr8  == &no_arg) goto done; args[n++] = &ptr8;
  if (&ptr9  == &no_arg) goto done; args[n++] = &ptr9;
  if (&ptr10 == &no_arg) goto done; args[n++] = &ptr10;
  if (&ptr11 == &no_arg) goto done; args[n++] = &ptr11;
  if (&ptr12 == &no_arg) goto done; args[n++] = &ptr12;
  if (&ptr13 == &no_arg) goto done; args[n++] = &ptr13;
  if (&ptr14 == &no_arg) goto done; args[n++] = &ptr14;
  if (&ptr15 == &no_arg) goto done; args[n++] = &ptr15;
  if (&ptr16 == &no_arg) goto done; args[n++] = &ptr16;
 done:

  int consumed;
  int vec[kVecSize];
  if (DoMatchImpl(*input, UNANCHORED, &consumed,
                  args, n, vec, kVecSize)) {
    input->remove_prefix(consumed);
    return true;
  } else {
    return false;
  }
}

bool RE::Replace(const StringPiece& rewrite,
                 string *str) const {
  int vec[kVecSize];
  int matches = TryMatch(*str, 0, UNANCHORED, vec, kVecSize);
  if (matches == 0)
    return false;

  string s;
  if (!Rewrite(&s, rewrite, *str, vec, matches))
    return false;

  assert(vec[0] >= 0);
  assert(vec[1] >= 0);
  str->replace(vec[0], vec[1] - vec[0], s);
  return true;
}

int RE::GlobalReplace(const StringPiece& rewrite,
                      string *str) const {
  int count = 0;
  int vec[kVecSize];
  string out;
  int start = 0;
  int lastend = -1;

  for (; start <= static_cast<int>(str->length()); count++) {
    int matches = TryMatch(*str, start, UNANCHORED, vec, kVecSize);
    if (matches <= 0)
      break;
    int matchstart = vec[0], matchend = vec[1];
    assert(matchstart >= start);
    assert(matchend >= matchstart);
    if (matchstart == matchend && matchstart == lastend) {
      // advance one character if we matched an empty string at the same
      // place as the last match occurred
      if (start < static_cast<int>(str->length()))
        out.push_back((*str)[start]);
      start++;
    } else {
      out.append(*str, start, matchstart - start);
      Rewrite(&out, rewrite, *str, vec, matches);
      start = matchend;
      lastend = matchend;
      count++;
    }
  }

  if (count == 0)
    return 0;

  if (start < static_cast<int>(str->length()))
    out.append(*str, start, str->length() - start);
  swap(out, *str);
  return count;
}

bool RE::Extract(const StringPiece& rewrite,
                 const StringPiece& text,
                 string *out) const {
  int vec[kVecSize];
  int matches = TryMatch(text, 0, UNANCHORED, vec, kVecSize);
  if (matches == 0)
    return false;
  out->erase();
  return Rewrite(out, rewrite, text, vec, matches);
}

/***** Actual matching and rewriting code *****/

int RE::TryMatch(const StringPiece& text,
                 int startpos,
                 Anchor anchor,
                 int *vec,
                 int vecsize) const {
  pcre* re = (anchor == ANCHOR_BOTH) ? re_full_ : re_partial_;
  if (re == NULL) {
    //fprintf(stderr, "Matching against invalid re: %s\n", error_->c_str());
    return 0;
  }

  pcre_extra extra = { 0 };
  if (options_.match_limit() > 0) {
    extra.flags = PCRE_EXTRA_MATCH_LIMIT;
    extra.match_limit = options_.match_limit();
  }
  int rc = pcre_exec(re,              // The regular expression object
                     &extra,
                     text.data(),
                     text.size(),
                     startpos,
                     (anchor == UNANCHORED) ? 0 : PCRE_ANCHORED,
                     vec,
                     vecsize);

  // Handle errors
  if (rc == PCRE_ERROR_NOMATCH) {
    return 0;
  } else if (rc < 0) {
    //fprintf(stderr, "Unexpected return code: %d when matching '%s'\n",
    //        re, pattern_.c_str());
    return 0;
  } else if (rc == 0) {
    // pcre_exec() returns 0 as a special case when the number of
    // capturing subpatterns exceeds the size of the vector.
    // When this happens, there is a match and the output vector
    // is filled, but we miss out on the positions of the extra subpatterns.
    rc = vecsize / 2;
  }

  if ((anchor == ANCHOR_BOTH) && (re_full_ == re_partial_)) {
    // We need an extra check to make sure that the match extended
    // to the end of the input string
    assert(vec[0] == 0);                 // PCRE_ANCHORED forces starting match
    if (vec[1] != text.size()) return 0; // Did not get ending match
  }

  return rc;
}

bool RE::DoMatchImpl(const StringPiece& text,
                     Anchor anchor,
                     int* consumed,
                     const Arg* const* args,
                     int n,
                     int* vec,
                     int vecsize) const {
  assert((1 + n) * 3 <= vecsize);  // results + PCRE workspace
  int matches = TryMatch(text, 0, anchor, vec, vecsize);
  assert(matches >= 0);  // TryMatch never returns negatives
  if (matches == 0)
    return false;

  *consumed = vec[1];

  if (args == NULL) {
    // We are not interested in results
    return true;
  }

  // If we got here, we must have matched the whole pattern.
  // We do not need (can not do) any more checks on the value of 'matches' here
  // -- see the comment for TryMatch.
  for (int i = 0; i < n; i++) {
    const int start = vec[2*(i+1)];
    const int limit = vec[2*(i+1)+1];
    if (!args[i]->Parse(text.data() + start, limit-start)) {
      // TODO: Should we indicate what the error was?
      return false;
    }
  }

  return true;
}

bool RE::DoMatch(const StringPiece& text,
                 Anchor anchor,
                 int* consumed,
                 const Arg* const args[],
                 int n) const {
  assert(n >= 0);
  size_t const vecsize = (1 + n) * 3;  // results + PCRE workspace
                                       // (as for kVecSize)
  int space[21];   // use stack allocation for small vecsize (common case)
  int* vec = vecsize <= 21 ? space : new int[vecsize];
  bool retval = DoMatchImpl(text, anchor, consumed, args, n, vec, vecsize);
  if (vec != space) delete [] vec;
  return retval;
}

bool RE::Rewrite(string *out, const StringPiece &rewrite,
                 const StringPiece &text, int *vec, int veclen) const {
  for (const char *s = rewrite.data(), *end = s + rewrite.size();
       s < end; s++) {
    int c = *s;
    if (c == '\\') {
      c = *++s;
      if (isdigit(c)) {
        int n = (c - '0');
        if (n >= veclen) {
          //fprintf(stderr, requested group %d in regexp %.*s\n",
          //        n, rewrite.size(), rewrite.data());
          return false;
        }
        int start = vec[2 * n];
        if (start >= 0)
          out->append(text.data() + start, vec[2 * n + 1] - start);
      } else if (c == '\\') {
        out->push_back('\\');
      } else {
        //fprintf(stderr, "invalid rewrite pattern: %.*s\n",
        //        rewrite.size(), rewrite.data());
        return false;
      }
    } else {
      out->push_back(c);
    }
  }
  return true;
}

// Return the number of capturing subpatterns, or -1 if the
// regexp wasn't valid on construction.
int RE::NumberOfCapturingGroups() {
  if (re_partial_ == NULL) return -1;

  int result;
  int pcre_retval = pcre_fullinfo(re_partial_,  // The regular expression object
                                  NULL,         // We did not study the pattern
                                  PCRE_INFO_CAPTURECOUNT,
                                  &result);
  assert(pcre_retval == 0);
  return result;
}

/***** Parsers for various types *****/

bool Arg::parse_null(const char* str, int n, void* dest) {
  // We fail if somebody asked us to store into a non-NULL void* pointer
  return (dest == NULL);
}

bool Arg::parse_string(const char* str, int n, void* dest) {
  reinterpret_cast<string*>(dest)->assign(str, n);
  return true;
}

bool Arg::parse_stringpiece(const char* str, int n, void* dest) {
  reinterpret_cast<StringPiece*>(dest)->set(str, n);
  return true;
}

bool Arg::parse_char(const char* str, int n, void* dest) {
  if (n != 1) return false;
  *(reinterpret_cast<char*>(dest)) = str[0];
  return true;
}

bool Arg::parse_uchar(const char* str, int n, void* dest) {
  if (n != 1) return false;
  *(reinterpret_cast<unsigned char*>(dest)) = str[0];
  return true;
}

// Largest number spec that we are willing to parse
static const int kMaxNumberLength = 32;

// REQUIRES "buf" must have length at least kMaxNumberLength+1
// REQUIRES "n > 0"
// Copies "str" into "buf" and null-terminates if necessary.
// Returns one of:
//      a. "str" if no termination is needed
//      b. "buf" if the string was copied and null-terminated
//      c. "" if the input was invalid and has no hope of being parsed
static const char* TerminateNumber(char* buf, const char* str, int n) {
  if ((n > 0) && isspace(*str)) {
    // We are less forgiving than the strtoxxx() routines and do not
    // allow leading spaces.
    return "";
  }

  // See if the character right after the input text may potentially
  // look like a digit.
  if (isdigit(str[n]) ||
      ((str[n] >= 'a') && (str[n] <= 'f')) ||
      ((str[n] >= 'A') && (str[n] <= 'F'))) {
    if (n > kMaxNumberLength) return ""; // Input too big to be a valid number
    memcpy(buf, str, n);
    buf[n] = '\0';
    return buf;
  } else {
    // We can parse right out of the supplied string, so return it.
    return str;
  }
}

bool Arg::parse_long_radix(const char* str,
                           int n,
                           void* dest,
                           int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, str, n);
  char* end;
  errno = 0;
  long r = strtol(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  *(reinterpret_cast<long*>(dest)) = r;
  return true;
}

bool Arg::parse_ulong_radix(const char* str,
                            int n,
                            void* dest,
                            int radix) {
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, str, n);
  char* end;
  errno = 0;
  unsigned long r = strtoul(str, &end, radix);
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  *(reinterpret_cast<unsigned long*>(dest)) = r;
  return true;
}

bool Arg::parse_short_radix(const char* str,
                            int n,
                            void* dest,
                            int radix) {
  long r;
  if (!parse_long_radix(str, n, &r, radix)) return false; // Could not parse
  if (r < SHRT_MIN || r > SHRT_MAX) return false;       // Out of range
  *(reinterpret_cast<short*>(dest)) = r;
  return true;
}

bool Arg::parse_ushort_radix(const char* str,
                             int n,
                             void* dest,
                             int radix) {
  unsigned long r;
  if (!parse_ulong_radix(str, n, &r, radix)) return false; // Could not parse
  if (r > USHRT_MAX) return false;                      // Out of range
  *(reinterpret_cast<unsigned short*>(dest)) = r;
  return true;
}

bool Arg::parse_int_radix(const char* str,
                          int n,
                          void* dest,
                          int radix) {
  long r;
  if (!parse_long_radix(str, n, &r, radix)) return false; // Could not parse
  if (r < INT_MIN || r > INT_MAX) return false;         // Out of range
  *(reinterpret_cast<int*>(dest)) = r;
  return true;
}

bool Arg::parse_uint_radix(const char* str,
                           int n,
                           void* dest,
                           int radix) {
  unsigned long r;
  if (!parse_ulong_radix(str, n, &r, radix)) return false; // Could not parse
  if (r > UINT_MAX) return false;                       // Out of range
  *(reinterpret_cast<unsigned int*>(dest)) = r;
  return true;
}

bool Arg::parse_longlong_radix(const char* str,
                               int n,
                               void* dest,
                               int radix) {
#ifndef HAVE_LONG_LONG
  return false;
#else
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, str, n);
  char* end;
  errno = 0;
#if defined HAVE_STRTOQ
  long long r = strtoq(str, &end, radix);
#elif defined HAVE_STRTOLL
  long long r = strtoll(str, &end, radix);
#else
#error parse_longlong_radix: cannot convert input to a long-long
#endif
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  *(reinterpret_cast<long long*>(dest)) = r;
  return true;
#endif   /* HAVE_LONG_LONG */
}

bool Arg::parse_ulonglong_radix(const char* str,
                                int n,
                                void* dest,
                                int radix) {
#ifndef HAVE_UNSIGNED_LONG_LONG
  return false;
#else
  if (n == 0) return false;
  char buf[kMaxNumberLength+1];
  str = TerminateNumber(buf, str, n);
  char* end;
  errno = 0;
#if defined HAVE_STRTOQ
  unsigned long long r = strtouq(str, &end, radix);
#elif defined HAVE_STRTOLL
  unsigned long long r = strtoull(str, &end, radix);
#else
#error parse_ulonglong_radix: cannot convert input to a long-long
#endif
  if (end != str + n) return false;   // Leftover junk
  if (errno) return false;
  *(reinterpret_cast<unsigned long long*>(dest)) = r;
  return true;
#endif   /* HAVE_UNSIGNED_LONG_LONG */
}

bool Arg::parse_double(const char* str, int n, void* dest) {
  if (n == 0) return false;
  static const int kMaxLength = 200;
  char buf[kMaxLength];
  if (n >= kMaxLength) return false;
  memcpy(buf, str, n);
  buf[n] = '\0';
  errno = 0;
  char* end;
  double r = strtod(buf, &end);
  if (end != buf + n) return false;   // Leftover junk
  if (errno) return false;
  *(reinterpret_cast<double*>(dest)) = r;
  return true;
}

bool Arg::parse_float(const char* str, int n, void* dest) {
  double r;
  if (!parse_double(str, n, &r)) return false;
  *(reinterpret_cast<float*>(dest)) = static_cast<float>(r);
  return true;
}


#define DEFINE_INTEGER_PARSERS(name)                                    \
  bool Arg::parse_##name(const char* str, int n, void* dest) {          \
    return parse_##name##_radix(str, n, dest, 10);                      \
  }                                                                     \
  bool Arg::parse_##name##_hex(const char* str, int n, void* dest) {    \
    return parse_##name##_radix(str, n, dest, 16);                      \
  }                                                                     \
  bool Arg::parse_##name##_octal(const char* str, int n, void* dest) {  \
    return parse_##name##_radix(str, n, dest, 8);                       \
  }                                                                     \
  bool Arg::parse_##name##_cradix(const char* str, int n, void* dest) { \
    return parse_##name##_radix(str, n, dest, 0);                       \
  }

DEFINE_INTEGER_PARSERS(short);
DEFINE_INTEGER_PARSERS(ushort);
DEFINE_INTEGER_PARSERS(int);
DEFINE_INTEGER_PARSERS(uint);
DEFINE_INTEGER_PARSERS(long);
DEFINE_INTEGER_PARSERS(ulong);
DEFINE_INTEGER_PARSERS(longlong);
DEFINE_INTEGER_PARSERS(ulonglong);

#undef DEFINE_INTEGER_PARSERS

}   // namespace pcrecpp
