--TEST--
OSS-Fuzz #472563272
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
false || (true ? true : false) || (false ? true : false) || true;
?>
===DONE===
--EXPECT--
===DONE===
