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

#include <vector>
#include <algorithm>     // for count()
#include <assert.h>
#include "config.h"
#include "pcre_scanner.h"

using std::vector;

namespace pcrecpp {

Scanner::Scanner()
  : data_(),
    input_(data_),
    skip_(NULL),
    should_skip_(false),
    save_comments_(false),
    comments_(NULL),
    comments_offset_(0) {
}

Scanner::Scanner(const string& in)
  : data_(in),
    input_(data_),
    skip_(NULL),
    should_skip_(false),
    save_comments_(false),
    comments_(NULL),
    comments_offset_(0) {
}

Scanner::~Scanner() {
  delete skip_;
  delete comments_;
}

void Scanner::Skip(const char* re) {
  delete skip_;
  if (re != NULL) {
    skip_ = new RE(re);
    should_skip_ = true;
    ConsumeSkip();
  } else {
    skip_ = NULL;
    should_skip_ = false;
  }
}

void Scanner::DisableSkip() {
  assert(skip_ != NULL);
  should_skip_ = false;
}

void Scanner::EnableSkip() {
  assert(skip_ != NULL);
  should_skip_ = true;
  ConsumeSkip();
}

int Scanner::LineNumber() const {
  // TODO: Make it more efficient by keeping track of the last point
  // where we computed line numbers and counting newlines since then.
  return 1 + std::count(data_.data(), input_.data(), '\n');
}

int Scanner::Offset() const {
  return input_.data() - data_.c_str();
}

bool Scanner::LookingAt(const RE& re) const {
  int consumed;
  return re.DoMatch(input_, RE::ANCHOR_START, &consumed, 0, 0);
}


bool Scanner::Consume(const RE& re,
                      const Arg& arg0,
                      const Arg& arg1,
                      const Arg& arg2) {
  const bool result = re.Consume(&input_, arg0, arg1, arg2);
  if (result && should_skip_) ConsumeSkip();
  return result;
}

// helper function to consume *skip_ and honour save_comments_
void Scanner::ConsumeSkip() {
  if (save_comments_) {
    if (NULL == comments_) {
      comments_ = new vector<StringPiece>;
    }
    const char *start_data = input_.data();
    skip_->Consume(&input_);
    // already pointing one past end, so no need to +1
    int length = input_.data() - start_data;
    if (length > 0) {
      comments_->push_back(StringPiece(start_data, length));
    }
  } else {
    skip_->Consume(&input_);
  }
}


void Scanner::GetComments(int start, int end, vector<StringPiece> *ranges) {
  // short circuit out if we've not yet initialized comments_
  // (e.g., when save_comments is false)
  if (!comments_) {
    return;
  }
  // TODO: if we guarantee that comments_ will contain StringPieces
  // that are ordered by their start, then we can do a binary search
  // for the first StringPiece at or past start and then scan for the
  // ones contained in the range, quit early (use equal_range or
  // lower_bound)
  for (vector<StringPiece>::const_iterator it = comments_->begin();
       it != comments_->end(); ++it) {
    if ((it->data() >= data_.c_str() + start &&
         it->data() + it->size() <= data_.c_str() + end)) {
      ranges->push_back(*it);
    }
  }
}


void Scanner::GetNextComments(vector<StringPiece> *ranges) {
  // short circuit out if we've not yet initialized comments_
  // (e.g., when save_comments is false)
  if (!comments_) {
    return;
  }
  for (vector<StringPiece>::const_iterator it =
         comments_->begin() + comments_offset_;
       it != comments_->end(); ++it) {
    ranges->push_back(*it);
    ++comments_offset_;
  }
}

}   // namespace pcrecpp
