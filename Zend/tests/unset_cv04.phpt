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

Notice: Undefined variable: x in %sunset_cv04.php on line %d
