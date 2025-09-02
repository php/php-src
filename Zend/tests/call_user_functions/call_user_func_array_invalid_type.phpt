--TEST--
call_user_func_array() generating TypeError
--FILE--
<?php
class drv {
    function func() {
    }
}

$drv = new drv;
try {
    call_user_func_array(array($drv, 'func'), null);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
call_user_func_array(): Argument #2 ($args) must be of type array, null given
