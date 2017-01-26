--TEST--
mb_check_encoding() - Circular references
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
ini_set('default_charset', 'UTF-8');
// Valid - Detects recursion
$str = "Japanese UTF-8 text. 日本語のUTF-8テキスト";
$arr = [1234, 12.34, TRUE, FALSE, NULL, $str, 'key'=>$str, $str=>'val'];
$tmp = &$arr;
$arr[] = $tmp;
var_dump(mb_check_encoding($str), mb_check_encoding($arr));

// Invalid - Return false due to short circuit check
$str = "Japanese UTF-8 text. 日本語\xFE\x01\x02のUTF-8テキスト";
$arr1 = [1234, 12.34, TRUE, FALSE, NULL, 'key'=>$str, $str=>'val'];
$tmp = &$arr1;
$arr1[] = $tmp;
$arr2 = [1234, 12.34, TRUE, FALSE, NULL, $str=>'val'];
$tmp = &$arr2;
$arr2[] = $tmp;
var_dump(mb_check_encoding($str), mb_check_encoding($arr1),  mb_check_encoding($arr2));
?>
--EXPECTF--
Warning: mb_check_encoding(): Cannot not handle circular references in %s on line %d
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
