--TEST--
Multiple use of @ operator must not fail to suppress diagnostic
--FILE--
<?php

function test1() {
    throw new Exception();
    return true;
}

$var = @@test1();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
