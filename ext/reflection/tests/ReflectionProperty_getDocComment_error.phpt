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
Warning: ReflectionProperty::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionProperty::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionProperty::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: ReflectionProperty::getDocComment() expects exactly 0 parameters, 1 given in %s on line %d
NULL
