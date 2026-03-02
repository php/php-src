--TEST--
ReflectionProperty::get{Hook,Hooks}() crashes on dynamic properties
--FILE--
<?php

#[\AllowDynamicProperties]
class MyDynamicClass {

}

class NonDynamicClass {

}

$cases = [ MyDynamicClass::class, stdClass::class, NonDynamicClass::class ];

foreach ( $cases as $c ) {
    echo "$c:" . PHP_EOL;
    $obj = new $c();
    $obj->prop = 'foo';
    $prop = new ReflectionProperty($obj, 'prop');
    var_dump( $prop->getHooks() );
    var_dump( $prop->getHook( PropertyHookType::Get ) );
    var_dump( $prop->getHook( PropertyHookType::Set ) );
    echo PHP_EOL;
}

?>
--EXPECTF--
MyDynamicClass:
array(0) {
}
NULL
NULL

stdClass:
array(0) {
}
NULL
NULL

NonDynamicClass:

Deprecated: Creation of dynamic property NonDynamicClass::$prop is deprecated in %sgh15718.php on line %d
array(0) {
}
NULL
NULL
