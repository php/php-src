--TEST--
Function overloading test 1
--SKIPIF--
<?php
	extension_loaded('mbstring') or die('skip mbstring not available');
	if (!function_exists("mail")) {
		die('skip mail() function is not available.');
	}
?>
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
--EXPECT--
Deprecated: The mbstring.func_overload directive is deprecated in Unknown on line 0
EUC-JP
int(33)
int(33)
