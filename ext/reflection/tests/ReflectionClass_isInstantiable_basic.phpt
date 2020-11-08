--TEST--
ReflectionClass::IsInstantiable()
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

$classes = array("C", "iface", "ifaceImpl", "abstractClass", "D");

foreach($classes  as $class ) {
    $reflectionClass = new ReflectionClass($class);
    echo "Is $class instantiable?  ";
    var_dump($reflectionClass->IsInstantiable());

}

?>
--EXPECT--
Is C instantiable?  bool(true)
Is iface instantiable?  bool(false)
Is ifaceImpl instantiable?  bool(true)
Is abstractClass instantiable?  bool(false)
Is D instantiable?  bool(true)
