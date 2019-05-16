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
--EXPECTF--
Fatal error: Uncaught BadMethodCallException: tar-based phar "%s/bug71488.test" cannot be created, link "%s" is too long for format in %sbug71488.php:%d
Stack trace:%A