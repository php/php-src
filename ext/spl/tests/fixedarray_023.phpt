--TEST--
SPL: FixedArray: Infinite loop in var_export bugfix
--FILE--
<?php
call_user_func(function () {
    $x = new SplFixedArray(4);
    $x[0] = NAN; // Test NAN just in case this check is incorrectly refactored to use zend_is_identical
    $x[1] = 0.0;
    $x[2] = $x;
    $x[3] = $x;
    var_export($x);
    echo "\n";
    $x[1] = -0.0;
    debug_zval_dump($x);
});
?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line 8

Warning: var_export does not handle circular references in %s on line 8
SplFixedArray::__set_state(array(
   0 => NAN,
   1 => 0.0,
   2 => NULL,
   3 => NULL,
))
object(SplFixedArray)#2 (4) refcount(6){
  [0]=>
  float(NAN)
  [1]=>
  float(-0)
  [2]=>
  *RECURSION*
  [3]=>
  *RECURSION*
}