--TEST--
Test the basics to function iconv.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--EXTENSIONS--
iconv
--FILE--
<?php
setlocale(LC_ALL, "en_US.utf8");
$in_charset          = 'UTF-8';
$out_charset 	     = 'ASCII//TRANSLIT';
$string_to_translate = 'Žluťoučký kůň\n';

$string_out = iconv($in_charset, $out_charset, $string_to_translate);

var_dump($string_out);

setlocale(LC_ALL, "C");
$in_charset          = 'UTF-8';
$out_charset 	     = 'ASCII//TRANSLIT';
$string_to_translate = 'Žluťoučký kůň\n';

$string_out = iconv($in_charset, $out_charset, $string_to_translate);

var_dump($string_out);

setlocale(LC_ALL, "invalid");
$in_charset          = 'UTF-8';
$out_charset 	     = 'ASCII//TRANSLIT';
$string_to_translate = 'Žluťoučký kůň\n';

$string_out = iconv($in_charset, $out_charset, $string_to_translate);

var_dump($string_out);
?>
--EXPECT--
string(15) "Zlutoucky kun\n"
string(15) "Zlutoucky kun\n"
string(15) "Zlutoucky kun\n"
