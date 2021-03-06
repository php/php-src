--TEST--
Silence nested function call which throws with @ operator
--FILE--
<?php

function test1() {
    throw new Exception();
    return true;
}

function test2($a) {
    $a += 6;
    return test1();
}

$var = @test2(1);

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
