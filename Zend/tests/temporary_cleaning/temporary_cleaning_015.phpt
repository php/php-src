--TEST--
Attempt to free invalid structure (result of ROPE_INIT is not a zval)
--FILE--
<?php
set_error_handler(function () {
    throw new Exception();
});
$a = [];
$b = "";
try {
     echo "$a$b\n";
} catch (Exception $ex) {
}
?>
DONE
--EXPECTF--
Array

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, 'Array to string...', '%s', %d)
#1 {main}
  thrown in %s on line %d
