--TEST--
Phar::canCompress
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
/* check this works and actually returns the boolean value */
var_dump(Phar::canCompress() == (
    extension_loaded("zlib") || extension_loaded("bz2")
    ));
?>
--EXPECT--
bool(true)
