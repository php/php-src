--TEST--
mb_check_encoding()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
ini_set('default_charset', 'UTF-8');
// Valid
$str = "Japanese UTF-8 text. 日本語のUTF-8テキスト";
$arr = [1234, 12.34, TRUE, FALSE, NULL, $str, 'key'=>$str, $str=>'val'];
var_dump(mb_check_encoding($str), mb_check_encoding($arr));

// Invalid
$str = "Japanese UTF-8 text. 日本語\xFE\x01\x02のUTF-8テキスト";
$arr1 = [1234, 12.34, TRUE, FALSE, NULL, 'key'=>$str, $str=>'val'];
$arr2 = [1234, 12.34, TRUE, FALSE, NULL, $str=>'val'];
var_dump(mb_check_encoding($str), mb_check_encoding($arr1),  mb_check_encoding($arr2));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
