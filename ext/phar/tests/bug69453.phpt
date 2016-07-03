--TEST--
Phar: bug #69453: Memory Corruption in phar_parse_tarfile when entry filename starts with null
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$fname = dirname(__FILE__) . '/bug69453.tar.phar';
try {
$r = new Phar($fname, 0);
} catch(UnexpectedValueException $e) {
	echo $e;
}
?>

==DONE==
--EXPECTF--
exception 'UnexpectedValueException' with message 'phar error: "%s%ebug69453.tar.phar" is a corrupted tar file (checksum mismatch of file "")' in %s:%d
Stack trace:
#0 %s%ebug69453.php(%d): Phar->__construct('%s', 0)
#1 {main}
==DONE==
