--TEST--
function that is conditionally defined at runtime should not cause compiler error
--FILE--
<?php

use function bar\foo;

if (0) {
    function foo() {
    }
}

echo "Done";

?>
--EXPECTF--
Fatal error: Cannot declare function foo because the name is already in use in %s on line %d
