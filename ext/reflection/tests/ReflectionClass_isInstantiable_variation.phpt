--TEST--
ReflectionClass::IsInstantiable()
--FILE--
<?php
class noCtor {
}

class publicCtorNew {
    public function __construct() {}
}

class protectedCtorNew {
    protected function __construct() {}
}

class privateCtorNew {
    private function __construct() {}
}

$classes = array("noCtor", "publicCtorNew", "protectedCtorNew", "privateCtorNew");
foreach ($classes as $class) {
    $reflectionClass = new ReflectionClass($class);
    echo "Is $class instantiable?  ";
    var_dump($reflectionClass->IsInstantiable());
}

?>
--EXPECT--
Is noCtor instantiable?  bool(true)
Is publicCtorNew instantiable?  bool(true)
Is protectedCtorNew instantiable?  bool(false)
Is privateCtorNew instantiable?  bool(false)
