--TEST--
Try finally (function call in the finally block after exception)
--FILE--
<?php
function foo() {
    echo "4";
}
function bar() {
    try {
        echo "2";
        throw new Exception();
        echo "x";
    } catch (MyEx $ex) {
        echo "x";
    } finally {
        echo "3";
        foo();
        echo "5";
    }
}
try {
    echo "1";
    bar();
    echo "x";
} catch (Exception $ex) {
    echo "6";
}
echo "\n";
?>
--EXPECT--
123456
