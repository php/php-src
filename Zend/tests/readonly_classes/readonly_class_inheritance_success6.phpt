--TEST--
Non-readonly child of a readonly class can declare non-readonly properties indirectly via a trait
--FILE--
<?php

trait BarTrait
{
    public static $property1 = 1;
    public static int $property2 = 2;
    public $property3 = 3;
    public int $property4 = 4;
}

readonly class Foo
{
}

class Bar extends Foo
{
    use BarTrait;
}

$bar = new Bar();
Bar::$property1 = 2;
Bar::$property2 = 3;
$bar->property3 = 4;
$bar->property4 = 5;

var_dump(Bar::$property1);
var_dump(Bar::$property2);
var_dump($bar);

?>
--EXPECT--
int(2)
int(3)
object(Bar)#1 (2) {
  ["property3"]=>
  int(4)
  ["property4"]=>
  int(5)
}
