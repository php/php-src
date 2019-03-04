--TEST--
Bug #77586 Symbolic link names in tar-formatted phar must be less than 100 bytes.
--SKIPIF--
<?php if (!extension_loaded("phar") || true /* blocked by bug 65332 */) die("skip"); ?>
--FILE--
<?php
$dir = __DIR__."/bug77586";
$phar = new PharData($dir . "/bug77586.tar");
$phar->buildFromDirectory($dir . "/files");
?>
--CLEAN--
<?php
$dir = __DIR__."/bug77586";
unlink($dir . "/bug77586.tar");
?>
--EXPECTF--
Fatal error: Uncaught PharException: tar-based phar "%s/bug77586.tar" cannot be created, link "%s" is too long for format %s
Stack trace:
#0 %s/bug77586.php(%d): PharData->buildFromDirectory('%s')
#1 {main}
  thrown in %s/bug77586.php %s on line %d
