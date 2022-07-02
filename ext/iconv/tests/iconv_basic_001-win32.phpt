--TEST--
Test the basics to function iconv.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--EXTENSIONS--
iconv
--SKIPIF--
<?php if(substr(PHP_OS, 0, 3) != 'WIN' ) {die('skip windows only test');} ?>
--FILE--
<?php
$in_charset          = 'UTF-8';
$out_charset 	     = 'ASCII//TRANSLIT';
$string_to_translate = 'Žluťoučký kůň\n';

$string_out = iconv($in_charset, $out_charset, $string_to_translate);

var_dump($string_out);
?>
--EXPECT--
string(16) "Zlutouck'y kun\n"
