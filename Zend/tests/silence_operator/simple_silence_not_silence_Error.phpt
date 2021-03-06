--TEST--
Suppression operator must not suppres Error type Throwable errors
--FILE--
<?php

function test1() {
    throw new Error();
    return true;
}

$var = @test1();

var_dump($var);

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error in %s:4
Stack trace:
#0 %s(8): test1()
#1 {main}
  thrown in %s on line 4
