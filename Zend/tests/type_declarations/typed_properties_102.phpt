--TEST--
Compound assignment operator on static property holding ref
--FILE--
<?php declare(strict_types=1);

class Test {
    public static int $intProp = 123;
    public static $prop;
}

Test::$prop =& Test::$intProp;
try {
    Test::$prop .= "foobar";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(Test::$prop, Test::$intProp);
?>
--EXPECT--
Cannot assign string to reference held by property Test::$intProp of type int
int(123)
int(123)
