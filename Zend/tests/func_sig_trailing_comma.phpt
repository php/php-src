--TEST--
Trailing comma in function signatures
--FILE--
<?php

function test(
    $there,
    $are,
    $many,
    $params,
) {
    echo "Foo\n";
}

class Test {
    public function method(
        $there,
        $are,
        $many,
        $params,
    ) {
        echo "Foo\n";
    }
}

$func = function(
    $there,
    $are,
    $many,
    $params,
) {
    echo "Foo\n";
};

$func = fn(
    $there,
    $shouldnt,
    $be,
    $many,
    $params,
) => print "Foo\n";

?>
===DONE===
--EXPECT--
===DONE===
