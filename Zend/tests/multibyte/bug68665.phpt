--TEST--
Crash with Big5
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires mbstring extension");
}
?>
--INI--
zend.multibyte=1
internal_encoding=big5
--FILE--
<?php
echo '\'hello';
?>
--EXPECT--
'hello
