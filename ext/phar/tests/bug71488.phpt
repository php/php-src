--TEST--
Phar: bug #71488: Stack overflow when decompressing tar archives
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$p = new PharData(__DIR__."/bug71488.tar");
$newp = $p->decompress("test");
?>

DONE
--CLEAN--
<?php
@unlink(__DIR__."/bug71488.test");
?>
--EXPECT--
DONE