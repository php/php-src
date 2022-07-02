--TEST--
Bug #64482 (Opcodes for dynamic includes should not be cached)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
include 'bug64482.inc';
echo "\n";
include 'php://filter/read=string.toupper/resource=bug64482.inc';
echo "\n";
?>
--EXPECT--
Dynamic include
DYNAMIC INCLUDE
