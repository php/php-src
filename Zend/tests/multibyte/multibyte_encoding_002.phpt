--TEST--
Zend Multibyte and UTF-8 BOM
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
internal_encoding=iso-8859-1
--FILE--
﻿<?php
print "Hello World\n";
?>
===DONE===
--EXPECT--
Hello World
===DONE===
