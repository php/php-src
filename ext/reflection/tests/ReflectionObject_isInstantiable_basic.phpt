--TEST--
ReflectionObject::IsInstantiable() - basic function test
--FILE--
<?php
class C {
}

interface iface {
    function f1();
}

class ifaceImpl implements iface {
    function f1() {}
}

abstract class abstractClass {
    function f1() {}
    abstract function f2();
}

class D extends abstractClass {
    function f2() {}
}

$classes = array("C", "ifaceImpl", "D");

foreach($classes  as $class ) {
    $ro = new ReflectionObject(new $class);
    echo "Is $class instantiable?  ";
    var_dump($ro->IsInstantiable());
}
?>
--EXPECT--
Is C instantiable?  bool(true)
Is ifaceImpl instantiable?  bool(true)
Is D instantiable?  bool(true)
