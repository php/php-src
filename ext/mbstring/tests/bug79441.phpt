--TEST--
Bug #79441 Segfault in mb_chr() if internal encoding is unsupported
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

mb_internal_encoding("utf-7");
try {
    mb_chr(0xd800);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
mb_chr() does not support the "UTF-7" encoding
