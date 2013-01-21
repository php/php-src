--TEST--
Accessors (like all properties) are case sensitive
--FILE--
<?php

class Test {
    public $foo { get; set; }
    public $Foo { get; set; }
}

$test = new Test;
$test->foo = 1;
$test->Foo = 2;
var_dump($test->foo, $test->Foo);

?>
--EXPECT--
int(1)
int(2)
