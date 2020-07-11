--TEST--
Variable assignment in catch must respect typed references
--FILE--
<?php

class Test {
    public int $i = 42;
    public string $s = "str";
}

$test = new Test;

$ref =& $test->i;
try {
    try {
        throw new Exception("ex");
    } catch (Exception $ref) {
        echo "Unreachable\n";
    }
} catch (TypeError $e) {
    var_dump($test->i);
    echo $e . "\n\n";
}

$ref =& $test->s;
try {
    try {
        throw new Exception("ex");
    } catch (Exception $ref) {
        echo "Unreachable\n";
    }
} catch (TypeError $e) {
    var_dump($test->s);
    echo $e . "\n\n";
}

?>
--EXPECTF--
int(42)
TypeError: Cannot assign Exception to reference held by property Test::$i of type int in %s:%d
Stack trace:
#0 {main}

string(3) "str"
TypeError: Cannot assign Exception to reference held by property Test::$s of type string in %s:%d
Stack trace:
#0 {main}
