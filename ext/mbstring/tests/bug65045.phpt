--TEST--
Bug #65045: mb_convert_encoding breaks well-formed character
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

//declare(encoding = 'UTF-8');
mb_internal_encoding('UTF-8');

$str = "\xF0\xA4\xAD".  "\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2";
$expected = "\xEF\xBF\xBD"."\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2";

$str2 = "\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD";
$expected2 = "\xF0\xA4\xAD\xA2"."\xF0\xA4\xAD\xA2"."\xEF\xBF\xBD";

mb_substitute_character(0xFFFD);
var_dump(
    $expected === htmlspecialchars_decode(htmlspecialchars($str, ENT_SUBSTITUTE, 'UTF-8')),
    $expected2 === htmlspecialchars_decode(htmlspecialchars($str2, ENT_SUBSTITUTE, 'UTF-8')),
    $expected === mb_convert_encoding($str, 'UTF-8', 'UTF-8'),
    $expected2 === mb_convert_encoding($str2, 'UTF-8', 'UTF-8')
);
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
