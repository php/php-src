--TEST--
Phar: bug #71498: Out-of-Bound Read in phar_parse_zipfile()
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
try {
$p = new PharData(__DIR__."/bug71498.zip");
} catch(UnexpectedValueException $e) {
	echo $e->getMessage();
}
?>

DONE
--EXPECTF--
phar error: end of central directory not found in zip-based phar "%s%ebug71498.zip"
DONE
