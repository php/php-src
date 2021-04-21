--TEST--
Accessor reflection
--FILE--
<?php

class Test {
    public $prop1;
    public $prop2 = 3.141 { get; private set; }
    public $prop3 {
        get { echo "get\n"; }
        set { echo "set($value)\n"; }
    }
}

$test = new Test;

$rp1 = new ReflectionProperty(Test::class, 'prop1');
var_dump($rp1->getGet());
var_dump($rp1->getSet());
echo "\n";

$rp2 = new ReflectionProperty(Test::class, 'prop2');
var_dump($g = $rp2->getGet());
var_dump($s = $rp2->getSet());
// TODO: This is broken.
var_dump($g->invoke($test));
try {
    $s->invoke($test, 42);
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
$s->setAccessible(true);
$s->invoke($test, 42);
var_dump($test->prop2);
echo "\n";

$rp3 = new ReflectionProperty(Test::class, 'prop3');
var_dump($g = $rp3->getGet());
var_dump($s = $rp3->getSet());
$g->invoke($test);
$s->invoke($test, 42);

?>
--EXPECT--
NULL
NULL

object(ReflectionMethod)#4 (2) {
  ["name"]=>
  string(11) "$prop2::get"
  ["class"]=>
  string(4) "Test"
}
object(ReflectionMethod)#5 (2) {
  ["name"]=>
  string(11) "$prop2::set"
  ["class"]=>
  string(4) "Test"
}
NULL
Trying to invoke private method Test::$prop2::set() from scope ReflectionMethod
float(3.141)

object(ReflectionMethod)#8 (2) {
  ["name"]=>
  string(11) "$prop3::get"
  ["class"]=>
  string(4) "Test"
}
object(ReflectionMethod)#4 (2) {
  ["name"]=>
  string(11) "$prop3::set"
  ["class"]=>
  string(4) "Test"
}
get
set(42)
