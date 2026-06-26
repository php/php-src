--TEST--
Reading an undefined parent CV from a scope-fn body emits the warning with the correct variable name
--FILE--
<?php
function outer() {
    /* $i is implicitly declared as a parent CV when the scope-fn body
     * references it; outer() never assigns it. The body's reference
     * triggers an "Undefined variable" warning — historically this
     * crashed because zval_undefined_cv looked up the var name in the
     * scope-fn's own op_array.vars instead of walking to the parent. */
    $fns = [];
    $fns[0] = fn() { return $i; };
    $fns[1] = fn() { return $i; };
    $fns[2] = fn() { return $i; };
    var_dump($fns[0]());

    /* Also exercise the warning path for a different parent var name to
     * confirm the lookup tracks the parent op_array's vars table. */
    $fns[3] = fn() { return $missing_param; };
    var_dump($fns[3]());
}
outer();
echo "done\n";
?>
--EXPECTF--
Warning: Undefined variable $i in %sundefined_parent_var.php on line %d
NULL

Warning: Undefined variable $missing_param in %sundefined_parent_var.php on line %d
NULL
done
