--TEST--
ReflectionParameter::getDefaultValueString() returns the string representation of default values
--FILE--
<?php

define("PI", 3.14);

class Foo
{
    public const BAR = "foobar";
}

function foo($a, $b = 0, $c = 3.14, $d = "string", $e = ["foo" => "bar"], $f = false, $g = PI, $h = Foo::BAR, $i = 3.14 * 4, $j = null)
{
}

$function = new ReflectionFunction('foo');

foreach ($function->getParameters() as $parameter) {
    try {
        var_dump($parameter->getDefaultValueString());
    } catch (ReflectionException $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
Internal error: Failed to retrieve the default value
string(1) "0"
string(4) "3.14"
string(8) "'string'"
string(16) "['foo' => 'bar']"
string(5) "false"
string(2) "PI"
string(8) "Foo::BAR"
string(8) "3.14 * 4"
string(4) "null"
