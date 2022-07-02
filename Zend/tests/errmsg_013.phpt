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
Fatal error: Cannot use string as default value for parameter $a of type array in %s on line %d
