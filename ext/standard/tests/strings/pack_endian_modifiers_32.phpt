--TEST--
pack()/unpack() endianness modifiers on 64-bit format codes, 32-bit systems
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) die("skip 32bit test only");
?>
--FILE--
<?php

$formats = ['q<', 'q>', 'Q<', 'Q>'];
foreach ($formats as $fmt) {
    try {
        pack($fmt, 0);
        echo "FAIL: Expected ValueError for pack('$fmt', 0)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach ($formats as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
pack('q<'): 64-bit format codes are not available for 32-bit versions of PHP
pack('q>'): 64-bit format codes are not available for 32-bit versions of PHP
pack('Q<'): 64-bit format codes are not available for 32-bit versions of PHP
pack('Q>'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('q<'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('q>'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('Q<'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('Q>'): 64-bit format codes are not available for 32-bit versions of PHP
