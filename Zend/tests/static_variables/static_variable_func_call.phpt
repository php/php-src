--TEST--
Static variable initializer with function call
--FILE--
<?php

function bar() {
    echo "bar() called\n";
    return 'bar';
}

function foo() {
    static $bar = bar();
    echo $bar, "\n";
}

foo();
foo();

?>
--EXPECT--
bar() called
bar
bar
