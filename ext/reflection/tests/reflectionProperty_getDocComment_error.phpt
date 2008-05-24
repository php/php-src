--TEST--
Test ReflectionProperty::getDocComment() errors.
--FILE--
<?php

class C {
    public $a;
}

$rc = new ReflectionProperty('C', 'a');
var_dump($rc->getDocComment(null));
var_dump($rc->getDocComment('X'));
var_dump($rc->getDocComment(true));
var_dump($rc->getDocComment(array(1, 2, 3)));

?>
--EXPECTF--

Warning: Wrong parameter count for ReflectionProperty::getDocComment() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::getDocComment() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::getDocComment() in %s on line %d
NULL

Warning: Wrong parameter count for ReflectionProperty::getDocComment() in %s on line %d
NULL
