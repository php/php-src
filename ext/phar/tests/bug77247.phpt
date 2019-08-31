--TEST--
PHP bug #77247 (heap buffer overflow in phar_detect_phar_fname_ext)
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
var_dump(new Phar('a/.b', 0,'test.phar'));
} catch(UnexpectedValueException $e) {
	echo "OK";
}
?>
--EXPECT--
OK