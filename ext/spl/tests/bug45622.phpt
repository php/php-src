--TEST--
SPL: Bug #45622 (isset($arrayObject->p) misbehaves with ArrayObject::ARRAY_AS_PROPS set
--FILE--
<?php

class C extends ArrayObject {
    public $p = 'object property';
}

$ao = new C(array('p'=>'array element'));
$ao->setFlags(ArrayObject::ARRAY_AS_PROPS);

echo "\n--> Access the real property:\n";
var_dump(isset($ao->p));
var_dump($ao->p);

echo "\n--> Remove the real property and access the array element:\n";
unset($ao->p);
var_dump(isset($ao->p));
var_dump($ao->p);

echo "\n--> Remove the array element and try access again:\n";
unset($ao->p);
var_dump(isset($ao->p));
var_dump($ao->p);

echo "\n--> Re-add the real property:\n";
$ao->p = 'object property';
var_dump(isset($ao->p));
var_dump($ao->p);
?>
--EXPECTF--
--> Access the real property:
bool(true)
string(15) "object property"

--> Remove the real property and access the array element:
bool(true)
string(13) "array element"

--> Remove the array element and try access again:
bool(false)

Warning: Undefined array key "p" in %s on line %d
NULL

--> Re-add the real property:
bool(true)
string(15) "object property"
