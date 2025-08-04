--TEST--
Global variable import using a name with side effects
--FILE--
<?php

function sf($arg) {
    echo "called\n";
    return $arg;
}

function test() {
    global ${sf("a")};
    var_dump($a);
}

$a = 42;
test();

?>
--EXPECT--
called
int(42)
