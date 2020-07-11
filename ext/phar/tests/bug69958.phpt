--TEST--
Phar: bug #69958: Segfault in Phar::convertToData on invalid file
--XFAIL--
Still has memory leaks, see https://bugs.php.net/bug.php?id=70005
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$tarphar = new PharData(__DIR__.'/bug69958.tar');
$phar = $tarphar->convertToData(Phar::TAR);
--EXPECTF--
Fatal error: Uncaught exception 'BadMethodCallException' with message 'phar "%s/bug69958.tar" exists and must be unlinked prior to conversion' in %s/bug69958.php:%d
Stack trace:
#0 %s/bug69958.php(%d): PharData->convertToData(%d)
#1 {main}
  thrown in %s/bug69958.php on line %d
