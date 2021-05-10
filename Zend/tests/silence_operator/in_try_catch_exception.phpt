--TEST--
Use of @ in try catch must not fail when an exception is thrown
--FILE--
<?php

function test1() {
    throw new Exception();
    return true;
}

try {
    $var = @test1();
} catch (\Throwable $e) {
    echo $e::class, \PHP_EOL;
}

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
