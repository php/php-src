--TEST--
Bug #29944 (Function defined in switch, crashes)
--FILE--
<?php
$a = 1;
switch ($a) {
  case 1:
    function foo($a) {
      return "ok\n";
    }
    echo foo($a);
}
?>
--EXPECT--
ok

