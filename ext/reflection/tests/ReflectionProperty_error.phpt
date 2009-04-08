--TEST--
Test ReflectionProperty class errors.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

class C {
    public static $p;
}

var_dump(new ReflectionProperty());
var_dump(new ReflectionProperty('C::p'));
var_dump(new ReflectionProperty('C', 'p', 'x'));
$rp = new ReflectionProperty('C', 'p');
var_dump($rp->getName(1));
var_dump($rp->isPrivate(1));
var_dump($rp->isProtected(1));
var_dump($rp->isPublic(1));
var_dump($rp->isStatic(1));
var_dump($rp->getModifiers(1));
var_dump($rp->isDefault(1));

?>
--EXPECTF--

Warning: ReflectionProperty::__construct() expects exactly 2 parameters, 0 given in %s on line %d
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(0) ""
  ["class"]=>
  string(0) ""
}

Warning: ReflectionProperty::__construct() expects exactly 2 parameters, 1 given in %s on line %d
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(0) ""
  ["class"]=>
  string(0) ""
}

Warning: ReflectionProperty::__construct() expects exactly 2 parameters, 3 given in %s on line %d
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(0) ""
  ["class"]=>
  string(0) ""
}

Warning: Wrong parameter count for ReflectionProperty::getName() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::isPrivate() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::isProtected() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::isPublic() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::isStatic() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::getModifiers() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::isDefault() in %s on line %d
NULL
