--TEST--
Listing Error in silence throwable class list should suppress
--FILE--
<?php

function test1() {
    throw new Error();
    return true;
}

$var = @<Error>test1();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
