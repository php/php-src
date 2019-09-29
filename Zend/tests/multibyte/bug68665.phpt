--TEST--
Crash with Big5
--SKIPIF--
<?php
if (!in_array("zend.detect_unicode", array_keys(ini_get_all()))) {
  die("skip Requires configure --enable-zend-multibyte option");
}
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

===DONE===
--EXPECT--
'hello
===DONE===
