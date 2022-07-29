--TEST--
Suppression operator with class list, single
--FILE--
<?php

function test1() {
    throw new Exception();
    return true;
}

$var = @<Exception>test1();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
