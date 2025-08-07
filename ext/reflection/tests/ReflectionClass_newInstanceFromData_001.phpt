--TEST--
ReflectionClass::newInstanceFromData
--FILE--
<?php

class A
{
    public int $a;
    public string $b;

    public function __construct($c, $d)
    {
        echo "In constructor of class A\n";
    }
}

class B
{
    public int $a;
    public string $b;
}

class C
{
}

#[\AllowDynamicProperties]
class D
{
}


$rcA = new ReflectionClass('A');
$rcB = new ReflectionClass('B');
$rcC = new ReflectionClass('C');
$rcD = new ReflectionClass('D');

try
{
    $rcA->newInstanceFromData(['a' => 'bad', 'b' => 123], ['foo', 1337]);
}
catch(Throwable $e)
{
    echo "Exception: " . $e->getMessage() . "\n";
}

var_dump($rcA->newInstanceFromData(['a' => 123, 'b' => 'good'], ['foo', 1337]));

var_dump($rcB->newInstanceFromData(['a' => 123, 'b' => 'good']));

var_dump($rcC->newInstanceFromData(['a' => 123, 'b' => 'good']));

var_dump($rcC->newInstanceFromData([]));

var_dump($rcD->newInstanceFromData(['a' => 123, 'b' => 'good']));

?>
--EXPECTF--
Exception: Cannot assign string to property A::$a of type int
In constructor of class A
object(A)#5 (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
object(B)#5 (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}

Deprecated: Creation of dynamic property C::$a is deprecated in %s on line %d

Deprecated: Creation of dynamic property C::$b is deprecated in %s on line %d
object(C)#5 (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
object(C)#5 (0) {
}
object(D)#5 (2) {
  ["a"]=>
  int(123)
  ["b"]=>
  string(4) "good"
}
