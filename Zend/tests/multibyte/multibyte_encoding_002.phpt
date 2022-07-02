--TEST--
Zend Multibyte and UTF-8 BOM
--EXTENSIONS--
mbstring
--INI--
zend.multibyte=1
internal_encoding=iso-8859-1
--FILE--
﻿<?php
print "Hello World\n";
?>
--EXPECT--
Hello World
