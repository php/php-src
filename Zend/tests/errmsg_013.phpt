--TEST--
errmsg: default value for parameters with array type can only be an array or null
--FILE--
<?php

class test {
    function foo(array $a = "s") {
    }
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: test::foo(): Parameter #1 ($a) of type array cannot have a default value of type string in %s on line %d
