--TEST--
Phar: bug #69441: Buffer Overflow when parsing tar/zip/phar in phar_set_inode
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$fname = __DIR__ . '/bug69441.phar';
try {
$r = new Phar($fname, 0);
} catch(UnexpectedValueException $e) {
	echo $e;
}
?>

==DONE==
--EXPECTF--
UnexpectedValueException: phar error: end of central directory not found in zip-based phar "%sbug69441.phar" in %sbug69441.php:%d
Stack trace:
#0 %s%ebug69441.php(%d): Phar->__construct('%s', 0)
#1 {main}
==DONE==
