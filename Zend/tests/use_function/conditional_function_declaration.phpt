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
Fatal error: Cannot use function bar\foo as foo because the name is already in use in %s on line %d
