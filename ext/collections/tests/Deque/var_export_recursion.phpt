--TEST--
Collections\Deque: Handle edge cases in var_export for circular data
--INI--
error_reporting=E_ALL
display_errors=stderr
--FILE--
<?php
call_user_func(function () {
    $x = new Collections\Deque();
    $x[] = NAN; // Test NAN just in case this check is incorrectly refactored to use zend_is_identical
    $x[] = 0.0;
    $x[] = $x;
    $x[] = $x;
    var_export($x);
    echo "\n";
    $x[1] = -0.0;
    debug_zval_dump($x);
});
?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line 8
Warning: var_export does not handle circular references in %s on line 8
%SCollections\Deque::__set_state(array(
   0 => NAN,
   1 => 0.0,
   2 => NULL,
   3 => NULL,
))
object(Collections\Deque)#2 (4) refcount(6){
  [0]=>
  float(NAN)
  [1]=>
  float(-0)
  [2]=>
  *RECURSION*
  [3]=>
  *RECURSION*
}