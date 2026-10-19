--TEST--
GH-24206: Tracing JIT loses loop-carried CV when its loop PHI is reloaded from memory
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
--CREDITS--
Fabien Potencier and Symfony contributors
mercierj
--FILE--
<?php

// Reduced from Symfony Mime v7.4.13 CharacterStream (MIT).
// Copyright (c) Fabien Potencier and Symfony contributors.
// LICENSE (from https://github.com/symfony/mime/tree/v7.4.13):
// Copyright (c) 2010-present Fabien Potencier
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

final class Scanner {
    private const MAP = ["a" => 1, "\xc3" => 2, "\xa9" => 0];

    public static function scan(string $s): int {
        $n = strlen($s);
        $count = 0;
        for ($i = 0; $i < $n; ++$i) {
            $char = $s[$i];
            $size = self::MAP[$char];
            if ($size == 0) {
                return -1;
            }
            for ($j = 1; $j < $size; ++$j) {
            }
            $i += $j - 1;
            ++$count;
        }
        return $count;
    }
}

var_dump(Scanner::scan(str_repeat("a", 200) . "\xc3\xa9" . "aaa"));

?>
--EXPECT--
int(204)
