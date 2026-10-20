--TEST--
Phar: bug #71498: Out-of-Bound Read in phar_parse_zipfile()
--EXTENSIONS--
phar
--FILE--
<?php
try {
$p = new PharData(__DIR__."/bug71498.zip");
} catch(UnexpectedValueException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>

DONE
--EXPECTF--
UnexpectedValueException: phar error: end of central directory not found in zip-based phar "%s%ebug71498.zip"

DONE
