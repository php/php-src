--TEST--
Bug GH-8044 (var_export/debug_zval_dump HT_ASSERT_RC1 debug failure for SplFixedArray)
--FILE--
<?php
call_user_func(function () {
    $x = new SplFixedArray(1);
    $x[0] = $x;
    var_export($x); echo "\n";
    debug_zval_dump($x); echo "\n";
});
?>
--EXPECTF--
Warning: var_export does not handle circular references in %s on line 5
\SplFixedArray::__set_state(array(
   0 => NULL,
))
object(SplFixedArray)#2 (1) refcount(4){
  [0]=>
  *RECURSION*
}
