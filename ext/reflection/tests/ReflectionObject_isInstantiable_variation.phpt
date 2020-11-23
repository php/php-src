--TEST--
ReflectionObject::IsInstantiable() - variation - constructors
--FILE--
<?php

class noCtor {
    public static function reflectionObjectFactory() {
        return new ReflectionObject(new self);
    }
}

class publicCtorNew {
    public function __construct() {}
    public static function reflectionObjectFactory() {
        return new ReflectionObject(new self);
    }
}

class protectedCtorNew {
    protected function __construct() {}
    public static function reflectionObjectFactory() {
        return new ReflectionObject(new self);
    }
}

class privateCtorNew {
    private function __construct() {}
    public static function reflectionObjectFactory() {
        return new ReflectionObject(new self);
    }
}

$reflectionObjects = array(
        noCtor::reflectionObjectFactory(),
        publicCtorNew::reflectionObjectFactory(),
        protectedCtorNew::reflectionObjectFactory(),
        privateCtorNew::reflectionObjectFactory(),
    );

foreach ($reflectionObjects as $reflectionObject) {
    $name = $reflectionObject->getName();
    echo "Is $name instantiable? ";
    var_dump($reflectionObject->IsInstantiable());
}
?>
--EXPECT--
Is noCtor instantiable? bool(true)
Is publicCtorNew instantiable? bool(true)
Is protectedCtorNew instantiable? bool(false)
Is privateCtorNew instantiable? bool(false)
