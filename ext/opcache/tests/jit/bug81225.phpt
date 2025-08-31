--TEST--
Bug #81225: Wrong result with pow operator with JIT enabled
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php

function unsignedLong(int $offset): int
{
    $normalizedOffset = $offset % (2 ** 32);

    if ($normalizedOffset < 0) {
        $normalizedOffset += 2 ** 32;
    }

    return $normalizedOffset;
}

$offset = -0x100000000 + 2;

for ($i = 0; $i < 200; ++$i) {
    assert(unsignedLong($offset) === 2);
}
?>
OK
--EXPECT--
OK
