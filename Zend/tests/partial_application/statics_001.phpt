--TEST--
Partial application static variables shared
--FILE--
<?php
function foo() {
    static $var = 0;
    
    ++$var;

    return $var;
}

foo();

$foo = foo(...);

if ($foo() == 2) {
    echo "OK";
}
?>
--EXPECTF--
OK
