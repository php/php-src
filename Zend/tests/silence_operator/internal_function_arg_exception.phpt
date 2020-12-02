--TEST--
Silencing an internal function argument throwing an Exception
--FILE--
<?php

function foo() {
    throw new Exception();
    return 1;
}

var_dump(@foo());

echo "Done\n";
?>
--EXPECT--
NULL
Done
