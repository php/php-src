--TEST--
Bug #37676 (Reference warnings)
--FILE--
<?php
$int=5;
$bool=true;
$null=null;
$float=5.1;

$index_v=5;
$index_r=6;

$null_v = $null[$index_v];
$null_r =& $null[$index_r];
var_dump($null, $null_v, $null_r);

$int_v = $int[$index_v];
$int_r =& $int[$index_r];
var_dump($int, $int_v, $int_r);

$bool_v = $bool[$index_v];
$bool_r =& $bool[$index_r];
var_dump($bool, $bool_v, $bool_r);

$float_v = $float[$index_v];
$float_r =& $float[$index_r];
var_dump($float, $float_v, $float_r);

$null2 = null;
$null2_v = $null2[1][2][3];
$null2_r =& $null2[1][2][3];
var_dump($null2, $null2_v, $null2_r);
?>
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d
array(1) {
  [6]=>
  &NULL
}
NULL
NULL

Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Cannot use a scalar value as an array in %s on line %d

Notice: Undefined variable: int_r in %s on line %d
int(5)
NULL
NULL

Warning: Variable of type boolean does not accept array offsets in %s on line %d

Warning: Cannot use a scalar value as an array in %s on line %d

Notice: Undefined variable: bool_r in %s on line %d
bool(true)
NULL
NULL

Warning: Variable of type float does not accept array offsets in %s on line %d

Warning: Cannot use a scalar value as an array in %s on line %d

Notice: Undefined variable: float_r in %s on line %d
float(5.1)
NULL
NULL

Warning: Variable of type null does not accept array offsets in %s on line %d
array(1) {
  [1]=>
  array(1) {
    [2]=>
    array(1) {
      [3]=>
      &NULL
    }
  }
}
NULL
NULL
