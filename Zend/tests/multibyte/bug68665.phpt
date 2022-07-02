--TEST--
Crash with Big5
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
internal_encoding=big5
--FILE--
<?php
echo '\'hello';
?>
--EXPECT--
'hello
