--TEST--
Bug #79441 Segfault in mb_chr() if internal encoding is unsupported
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

mb_internal_encoding("utf-7");
mb_chr(0xd800);

?>
--EXPECTF--
Warning: mb_chr(): Unsupported encoding "UTF-7" in %s on line %d
