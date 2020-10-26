--TEST--
errmsg: can't use function return value in write context
--FILE--
<?php

function foo() {
    return "blah";
}

foo() = 1;

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot use function return value in write context in %s on line %d
