--TEST--
Silence function with ops before exception with @ operator
--FILE--
<?php

function test1() {
    $b = strlen("Hello");
    $a = 5;
    throw new Exception();
    $a += 25;
    return true;
}

$var = @test1();

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
