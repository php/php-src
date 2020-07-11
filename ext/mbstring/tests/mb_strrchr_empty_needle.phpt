--TEST--
Test mb_strrchr() function : with empty needle
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrchr') or die("skip mb_strrchr() is not available in this build");
?>
--FILE--
<?php

mb_internal_encoding('UTF-8');

$string_ascii = 'abc def';
// Japanese string in UTF-8
$string_mb = "日本語テキストです。01234５６７８９。";

echo "\n-- ASCII string --\n";
var_dump(mb_strrchr($string_ascii, '', false, 'ISO-8859-1'));
var_dump(mb_strrchr($string_ascii, ''));
var_dump(mb_strrchr($string_ascii, '', true));

echo "\n-- Multibyte string --\n";
var_dump(mb_strrchr($string_mb, ''));
var_dump(mb_strrchr($string_mb, '', false, 'utf-8'));
var_dump(mb_strrchr($string_mb, '', true));

?>
--EXPECT--
-- ASCII string --
string(0) ""
string(0) ""
string(7) "abc def"

-- Multibyte string --
string(0) ""
string(0) ""
string(53) "日本語テキストです。01234５６７８９。"
