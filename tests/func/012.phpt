--TEST--
Assert dynamic string variables can be imported to function scope with T_USE
--FILE--
<?php
${'+'} = function ($x, $y) { return $x + $y; };

$test_add = (function ($a, $b) use (${'+'}) {
  return ${'+'}($a, $b);
})(10, 20);

echo $test_add;
?>
--EXPECT--
30
