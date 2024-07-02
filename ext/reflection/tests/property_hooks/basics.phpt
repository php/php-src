--TEST--
Property hook reflection
--FILE--
<?php

class Test {
    public $prop1;
    public $prop2 { get {} set {} }
    public $prop3 {
        get { echo "get\n"; }
        set { echo "set($value)\n"; }
    }
}
abstract class Test2 {
    abstract public $prop4 { get; set; }
}

function dumpFlags(ReflectionProperty $rp) {
    $modifiers = $rp->getModifiers();
    echo "Abstract: ";
    echo $rp->isAbstract() ? "true" : "false";
    echo " ";
    echo $modifiers & ReflectionProperty::IS_ABSTRACT ? "true" : "false";
    echo "\n";
}

$test = new Test;

$rp1 = new ReflectionProperty(Test::class, 'prop1');
var_dump($rp1->getHook(PropertyHookType::Get));
var_dump($rp1->getHook(PropertyHookType::Set));
dumpFlags($rp1);
echo "\n";

$rp2 = new ReflectionProperty(Test::class, 'prop2');
var_dump($g = $rp2->getHook(PropertyHookType::Get));
var_dump($s = $rp2->getHook(PropertyHookType::Set));
var_dump($g->invoke($test));
try {
    $s->invoke($test, 42);
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}
$s->setAccessible(true);
$s->invoke($test, 42);
var_dump($test->prop2);
dumpFlags($rp2);
echo "\n";

$rp3 = new ReflectionProperty(Test::class, 'prop3');
var_dump($g = $rp3->getHook(PropertyHookType::Get));
var_dump($s = $rp3->getHook(PropertyHookType::Set));
$g->invoke($test);
$s->invoke($test, 42);
dumpFlags($rp3);
echo "\n";

$rp4 = new ReflectionProperty(Test2::class, 'prop4');
dumpFlags($rp4);

?>
--EXPECT--
NULL
NULL
Abstract: false false

object(ReflectionMethod)#6 (2) {
  ["name"]=>
  string(11) "$prop2::get"
  ["class"]=>
  string(4) "Test"
}
object(ReflectionMethod)#7 (2) {
  ["name"]=>
  string(11) "$prop2::set"
  ["class"]=>
  string(4) "Test"
}
NULL
NULL
Abstract: false false

object(ReflectionMethod)#9 (2) {
  ["name"]=>
  string(11) "$prop3::get"
  ["class"]=>
  string(4) "Test"
}
object(ReflectionMethod)#6 (2) {
  ["name"]=>
  string(11) "$prop3::set"
  ["class"]=>
  string(4) "Test"
}
get
set(42)
Abstract: false false

Abstract: true true
