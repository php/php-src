--TEST--
Zend Multibyte and ShiftJIS
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
internal_encoding=SJIS
--FILE--
<?php
declare(encoding='Shift_JIS');
$s = "•\"; // 0x95+0x5c in script, not somewhere else "
printf("%x:%x", ord($s[0]), ord($s[1]));
?>
--EXPECT--
95:5c
