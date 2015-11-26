--TEST--
function that is conditionally defined at runtime should not cause compiler error
--FILE--
<?php

if (0) {
    function foo() {
    }
}

use function bar\foo;

echo "Done";

?>
--EXPECT--
Done
