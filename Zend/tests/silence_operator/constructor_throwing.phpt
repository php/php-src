--TEST--
Silencing a new object instance
--FILE--
<?php

class A {
    public function __construct() {
        throw new Exception();
    }
}

$var = @new A();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
