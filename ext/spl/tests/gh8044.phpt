--TEST--
Bug GH-8044 (var_export HT_ASSERT_RC1 debug failure for SplFixedArray)
--FILE--
<?php
set_error_handler(function ($errno, $errmsg) {
    echo "errmsg=$errmsg\n";
});
$x = new SplFixedArray(2);
$x[0] = $x;
$x[1] = $x;
var_export($x);
?>
--EXPECT--
errmsg=var_export does not handle circular references
errmsg=var_export does not handle circular references
SplFixedArray::__set_state(array(
   0 => NULL,
   1 => NULL,
))
