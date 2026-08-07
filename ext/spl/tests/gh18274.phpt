--TEST--
GH-18274 (Assertion failure in SplFixedArray when an error handler runs during an undefined-variable write)
--FILE--
<?php
set_error_handler('func_get_args');
function foo() {
    for ($cnt = 0; $cnt < 6; $cnt++) {
        $b = new SplFixedArray(1);
        $b[0] = $a;
    }
}
@foo();
echo "done\n";
?>
--EXPECTF--
Fatal error: Uncaught ArgumentCountError: func_get_args() expects exactly 0 arguments, 4 given in %s:%d
Stack trace:
#%d %A: func_get_args(%d, %s, %s, %d)%A
#%d {main}
  thrown in %s on line %d
