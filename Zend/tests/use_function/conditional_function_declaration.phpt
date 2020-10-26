--TEST--
function that is conditionally defined is subject to symbol use checks
--FILE--
<?php

if (0) {
    function foo() {
    }
}

use function bar\foo;

echo "Done";

?>
--EXPECTF--
Fatal error: Cannot import function bar\foo as foo, foo has already been declared in %s on line %d
