--TEST--
Reflection Bug #29268 (__autoload() not called with reflectionProperty->getClass())
--FILE--
<?php
spl_autoload_register(function ($classname) {
    echo "__autoload($classname)\n";
    eval("class $classname {}");
});

class B{
    public function doit(A $a){
    }
}

$ref = new reflectionMethod('B','doit');
$parameters = $ref->getParameters();
foreach($parameters as $parameter)
{
    $class = $parameter->getClass();
    echo $class->name."\n";
}
echo "ok\n";
?>
--EXPECTF--
Deprecated: Method ReflectionParameter::getClass() is deprecated in %s on line %d
__autoload(A)
A
ok
