--TEST--
Bug #65845 (Error when Zend Opcache Optimizer is fully enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
$Pile['vars'][(string)'toto'] = 'tutu';
var_dump($Pile['vars']['toto']);
?>
--EXPECT--
string(4) "tutu"
