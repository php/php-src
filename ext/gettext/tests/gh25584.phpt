--TEST--
GH-25584 (JIT frees the string returned by gettext(), dgettext() and dcgettext() while still referenced)
--EXTENSIONS--
gettext
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
$s = str_repeat("a", mt_rand(3, 3));
gettext($s);
dgettext("x", $s);
dcgettext("x", $s, LC_CTYPE);
echo "msgid after calls: ";
var_dump($s);
?>
--EXPECT--
msgid after calls: string(3) "aaa"
