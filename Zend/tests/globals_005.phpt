--TEST--
$GLOBALS resize
--FILE--
<?php
function foo() {
  for ($i = 0; $i < 100; $i++) {
     $GLOBALS["A". $i]  = 1; //trigger resize
  }
  return "ops";
}

$GLOBALS[foo()] = "ops";
?>
DONE
--EXPECT--
DONE
