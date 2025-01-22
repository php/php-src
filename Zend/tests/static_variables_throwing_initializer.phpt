--TEST--
Static variable with throwing initializer
--FILE--
<?php

function foo($throw) {
    static $a = $throw ? (throw new Exception('Throwing from foo()')) : 42;
    return $a;
}

try {
    var_dump(foo(true));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump(foo(false));

?>
--EXPECT--
Throwing from foo()
int(42)
