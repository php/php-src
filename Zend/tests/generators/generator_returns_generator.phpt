--TEST--
A generator function returns a Generator object
--FILE--
<?php

function* foo() {
    // execution is suspended here, so the following never gets run:
    echo "Foo";
}

$generator = foo();
var_dump($generator instanceof Generator);

?>
--EXPECT--
bool(true)
