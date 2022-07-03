--TEST--
unset() CV 4 (unset() local variable in included file)
--FILE--
<?php
function f() {
  $x = "ok\n";
  echo $x;
  include "unset.inc";
  echo $x;
}
f();
?>
--EXPECTF--
ok

Warning: Undefined variable $x (This will become an error in PHP 9.0) in %s on line %d
