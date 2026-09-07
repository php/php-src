--TEST--
SPL: Bug #48493 spl_autoload_unregister() can't handle prepended functions
--FILE--
<?php
function autoload1() {}

function autoload2() {}

spl_autoload_register('autoload2');
spl_autoload_register('autoload1', true, true);
var_dump(spl_autoload_functions());

spl_autoload_unregister('autoload2');
var_dump(spl_autoload_functions());
?>
--EXPECT--
array(2) {
  [0]=>
  string(9) "autoload1"
  [1]=>
  string(9) "autoload2"
}
array(1) {
  [0]=>
  string(9) "autoload1"
}
