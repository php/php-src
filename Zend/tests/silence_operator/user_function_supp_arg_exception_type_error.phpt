--TEST--
Listed silencing a userland function supplementary argument throwing an Exception
--FILE--
<?php

function test1(int $v) {
    return $v;
}

function foo() {
    throw new Exception();
    return 1;
}

try {
    $var = test1(@<Exception>foo());
    var_dump($var);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "Done\n";
?>
--EXPECTF--
test1(): Argument #1 ($v) must be of type int, null given, called in %s on line %d
Done
