--TEST--
Phar: bug #69958: Segfault in Phar::convertToData on invalid file
--EXTENSIONS--
phar
--FILE--
<?php
$tarphar = new PharData(__DIR__.'/bug69958.tar');
$phar = $tarphar->convertToData(Phar::TAR);
?>
--EXPECTF--
Fatal error: Uncaught BadMethodCallException: phar "%sbug69958.tar" exists and must be unlinked prior to conversion in %sbug69958.php:%d
Stack trace:
#0 %sbug69958.php(%d): PharData->convertToData(%d)
#1 {main}
  thrown in %sbug69958.php on line %d
