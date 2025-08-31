--TEST--
Incorrect DCE of FREE
--FILE--
<?php
function foo() {
  $a = $r[] = $r = []&$y;
  list(&$y)=$a;
}
?>
DONE
--EXPECT--
DONE
