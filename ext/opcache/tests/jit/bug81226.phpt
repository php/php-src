--TEST--
Bug #81226: Integer overflow behavior is different with JIT enabled
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=tracing
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
// 65-bit hexadecimal number
$hex = '10000000000000041';

for ($i = 0; $i < 200; ++$i) {
    $characterReferenceCode = 0;

    for ($j = 0, $len = strlen($hex); $j < $len; ++$j) {
        $characterReferenceCode *= 16;
        $characterReferenceCode += ord($hex[$j]) - 0x0030;
    }

    assert($characterReferenceCode > 0x10FFFF);
}
?>
OK
--EXPECT--
OK
