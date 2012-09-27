--TEST--
SPL: ArrayObject::setFlags basic usage with ArrayObject::ARRAY_AS_PROPS. Currently fails on php.net due to bug 45622. 
--FILE--
<?php
class C extends ArrayObject {
	public $p = 'object property';
}	

function access_p($ao) {
  // isset
  var_dump(isset($ao->p));
  // read
  var_dump($ao->p);
  // write  
  $ao->p = $ao->p . '.changed';
  var_dump($ao->p);
}

$ao = new C(array('p'=>'array element'));
$ao->setFlags(ArrayObject::ARRAY_AS_PROPS);

echo "\n--> Access the real property:\n";
access_p($ao);

echo "\n--> Remove the real property and access the array element:\n";
unset($ao->p);
access_p($ao);

echo "\n--> Remove the array element and try access again:\n";
unset($ao->p);
access_p($ao);
?>
--EXPECTF--
--> Access the real property:
bool(true)
string(15) "object property"
string(23) "object property.changed"

--> Remove the real property and access the array element:
bool(true)
string(13) "array element"
string(21) "array element.changed"

--> Remove the array element and try access again:
bool(false)

Notice: Undefined index: p in %s on line 10
NULL

Notice: Undefined index: p in %s on line 12
string(8) ".changed"
