--TEST--
PFA: default value AST of an SHM-persisted partial
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--FILE--
<?php

// parse_ini_string() has a $scanner_mode param that defaults to INI_SCANNER_NORMAL,
// which is an IS_CONSTANT_AST.
$p = parse_ini_string(?, ...);
var_dump($p("a=1"));

$param = (new ReflectionFunction($p))->getParameters()[2];
var_dump($param->getDefaultValue());

?>
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "1"
}
int(0)
