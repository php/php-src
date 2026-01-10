--TEST--
First Class Callable preserve Called Scope
--FILE--
<?php
class Foo { 
    public static function method() { 
        return static::class;
    }
}

class Bar extends Foo {}

$bar = Bar::method(...);

echo $bar();
?>
--EXPECT--
Bar
