--TEST--
Phar - Bug #71625 - Crash in php7.dll
--INI--
phar.readonly=0
--SKIPIF--
<?php 

if (!extension_loaded("phar") || !extension_loaded("zlib")) die("skip");
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
    die('skip windows only test');
}

?>
--FILE--
<?php
$phar = new Phar("A:A:.phar");
$phar["hello_habr.txt"] = '<? Hello Habr!?>';
?>
DONE
--EXPECTF--
Fatal error: Uncaught UnexpectedValueException: Cannot create phar 'A:A:.phar', file extension (or combination) not recognised or the directory does not exist in %sbug71625.php:%d
Stack trace:
#0 %sbug71625.php(%d): Phar->__construct('A:A:.phar')
#1 {main}
  thrown in %sbug71625.php on line %d
