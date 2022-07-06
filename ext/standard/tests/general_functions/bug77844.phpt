--TEST--
Bug #77844: Crash due to null pointer in parse_ini_string with INI_SCANNER_TYPED
--FILE--
<?php

$ini = <<<INI
val1=3.7&2
val2=2&3.7
INI;
var_dump(parse_ini_string($ini, true, INI_SCANNER_TYPED));

?>
--EXPECT--
array(2) {
  ["val1"]=>
  string(1) "2"
  ["val2"]=>
  string(1) "2"
}
