--TEST--
GH-16821 (Spill conflict between ASSIGN.op1_def and ASSIGN.op2_use)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
namespace Foo; // Needed so ord() and strlen() don't compile to opcodes

// This code is heavily derived from phpseclib
// License:
// Copyright (c) 2011-2019 TerraFrost and other contributors
// 
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

function reduce(string $c, string $u): string {
    $c = str_pad($c, 143, "\0", STR_PAD_LEFT);
    for ($h = 5, $i = 1140; $i >= 571;) {
        $g = $h >> 3;
        for ($mask = 0x80; $mask > 0; $mask >>= 1, $i--, $h++) {
            if (ord($c[$g]) & $mask) {
                $temp = $i - 571;
                $j = $temp >> 3;
                $t1 = $j ? substr($c, 0, -$j) : $c;
                $length = strlen($t1);
                $temp = $t1 ^ str_pad($u, $length, "\0", STR_PAD_LEFT);
                $c = substr_replace($c, $temp, 0, $length);
            }
        }
    }
    return $c;
}

var_dump(md5(reduce(str_repeat("\xff", 72), "\x04\x25")));
?>
--EXPECT--
string(32) "dd12a5bb6e3d7b4abcdd92e135850169"

