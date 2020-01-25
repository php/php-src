--TEST--
Can call internal functions from parameter default
--FILE--
<?php
function evaluated_user_function() {
    // NOTE: PHP only caches non-refcounted values in the RECV_INIT value,
    // meaning that if the returned value is dynamic, this will get called every time.
    // TODO: Would it be worth it to convert refcounted values to immutable values?
    echo "Evaluating default\n";
    return sprintf("%s default", "Dynamic");
}
function test_default($x = evaluated_user_function()) {
    echo "x is $x\n";
}
test_default();
test_default(2);
test_default();
?>
--EXPECT--
Evaluating default
x is Dynamic default
x is 2
Evaluating default
x is Dynamic default