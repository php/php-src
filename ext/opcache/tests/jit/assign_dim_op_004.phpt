--TEST--
JIT ASSIGN_DIM_OP: Cyclic dependency
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$a = null; 
$a[] .= $a;
var_dump($a);
?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}
