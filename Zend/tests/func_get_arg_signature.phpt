--TEST--
Test func_get_args signature
--FILE--
<?php

(function () {
    // Pass null to avoid zend_try_compile_special_func and trigger the func_info check
    var_dump(func_get_args(null));
})();

?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: func_get_args() expects exactly 0 parameters, 1 given in %s:5
Stack trace:
#0 %s(5): func_get_args(NULL)
#1 %s(6): {closure}()
#2 {main}
  thrown in %s on line 5
