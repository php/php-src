--TEST--
SPL: ArrayObject::exchangeArray() basic usage with object as underlying data store.
--XFAIL--
Failing in 5.2 due to http://thread.gmane.org/gmane.comp.php.devel/52545/focus=52559
--FILE--
<?php

class C {
	public $pub1 = 'public1';
} 

echo "--> exchangeArray() with objects:\n";
$original = new C;
$ao = new ArrayObject($original);
$swapIn = new C;
try {
	$copy = $ao->exchangeArray($swapIn);
	$copy['addedToCopy'] = 'added To Copy';
} catch (Exception $e) {
	echo "Exception:" . $e->getMessage() . "\n";
}
$swapIn->addedToSwapIn = 'added To Swap-In';
$original->addedToOriginal = 'added To Original';
var_dump($ao, $original, $swapIn, $copy);


echo "\n\n--> exchangeArray() with no arg:\n";
unset($original, $ao, $swapIn, $copy);
$original = new C;
$ao = new ArrayObject($original);
try {
	$copy = $ao->exchangeArray();
	$copy['addedToCopy'] = 'added To Copy';
} catch (Exception $e) {
	echo "Exception:" . $e->getMessage() . "\n";
}
$original->addedToOriginal = 'added To Original';
var_dump($ao, $original, $copy);

echo "\n\n--> exchangeArray() with bad arg type:\n";
unset($original, $ao, $swapIn, $copy);
$original = new C;
$ao = new ArrayObject($original);
try {
	$copy = $ao->exchangeArray(null);
	$copy['addedToCopy'] = 'added To Copy';
} catch (Exception $e) {
	echo "Exception:" . $e->getMessage() . "\n";
}
$original->addedToOriginal = 'added To Original';
var_dump($ao, $original, $copy);

?>
--EXPECTF--
--> exchangeArray() with objects:
object(ArrayObject)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToSwapIn"]=>
  string(16) "added To Swap-In"
}
object(C)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
object(C)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToSwapIn"]=>
  string(16) "added To Swap-In"
}
array(2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToCopy"]=>
  string(13) "added To Copy"
}


--> exchangeArray() with no arg:

Warning: ArrayObject::exchangeArray() expects exactly 1 parameter, 0 given in %s on line 27
object(ArrayObject)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
object(C)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
array(2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToCopy"]=>
  string(13) "added To Copy"
}


--> exchangeArray() with bad arg type:
Exception:Passed variable is not an array or object, using empty array instead

Notice: Undefined variable: copy in %s on line 46
object(ArrayObject)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
object(C)#%d (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
NULL
