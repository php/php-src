--TEST--
Test the basics to function iconv.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
<?php if(substr(PHP_OS, 0, 3) == 'WIN' ) {die('skip not for windows');} ?>
<?php if(setlocale(LC_ALL, "en_US.utf8") === false) { die('skip en_US.utf8 locales not available'); } ?>
--FILE--
<?php
setlocale(LC_ALL, "en_US.utf8");
$in_charset          = 'UTF-8';
$out_charset 	     = 'ASCII//TRANSLIT';
$string_to_translate = 'Žluťoučký kůň\n';

$string_out = iconv($in_charset, $out_charset, $string_to_translate);

var_dump($string_out);
?>
--EXPECT--
string(15) "Zlutoucky kun\n"
