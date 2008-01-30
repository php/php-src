/* utf8_to_utf16.c */

/* 2005-12-25 */

/*
Copyright (c) 2005 JSON.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "utf8_to_utf16.h"
#include "utf8_decode.h"

int 
utf8_to_utf16(unsigned short w[], char p[], int length) 
{
    int c;
    int the_index = 0;
    json_utf8_decode utf8;
    
    utf8_decode_init(&utf8, p, length);
    for (;;) {
        c = utf8_decode_next(&utf8);
        if (c < 0) {
            return (c == UTF8_END) ? the_index : UTF8_ERROR;
        }
        if (c < 0x10000) {
            w[the_index] = (unsigned short)c;
            the_index += 1;
        } else {
            c &= 0xFFFF;
            w[the_index] = (unsigned short)(0xD800 | (c >> 10));
            the_index += 1;
            w[the_index] = (unsigned short)(0xDC00 | (c & 0x3FF));
            the_index += 1;
        }
    }
}
