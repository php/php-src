--TEST--
Function overloading test (said to be harmful)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.func_overload=7
mbstring.internal_encoding=EUC-JP
--FILE--
<?php
echo mb_internal_encoding()."\n";

$ngchars = array('Ç½','É½','»½','¥½');
$str = '¸µÏ½ÍÜ»½Ðò¼Òº¾µ½É½¸½Ç½ÎÏÉ½¼¨±½ÌÈÄäË½ÎÏÅ½ÉÕ¹½Ê¸·½»ÒÍ½ÃÎñ½Æ¬¥½¥Õ¥¡¡¼';
var_dump(strlen($str));
var_dump(mb_strlen($str));

$converted_str = mb_convert_encoding($str, 'Shift_JIS');
mb_regex_encoding('Shift_JIS');
foreach($ngchars as $c) {
	$c = mb_convert_encoding($c, 'Shift_JIS');
	$replaced = mb_convert_encoding(ereg_replace($c, '!!', $converted_str), mb_internal_encoding(), 'Shift_JIS');
	var_dump(strpos($replaced, '!!'));
}
?>
--EXPECT--
EUC-JP
int(33)
int(33)
int(10)
int(8)
int(3)
int(29)
