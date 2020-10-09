--TEST--
EUC-JP to UTF8
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
error_reporting=2039
--FILE--
<?php
/* charset=EUC-JP */

$str = "
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
���ܸ�ƥ����Ȥ�English Text
";

$str = iconv("EUC-JP", "UTF-8", $str); /* libiconv(1.8) doesn't know "UTF8" but "UTF-8". */
$str = base64_encode($str);
echo $str."\n";

?>
--EXPECT--
CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0CuaXpeacrOiqnuODhuOCreOCueODiOOBqEVuZ2xpc2ggVGV4dArml6XmnKzoqp7jg4bjgq3jgrnjg4jjgahFbmdsaXNoIFRleHQK5pel5pys6Kqe44OG44Kt44K544OI44GoRW5nbGlzaCBUZXh0Cg==
