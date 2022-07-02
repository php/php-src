--TEST--
SPL: ArrayObject::exchangeArray() basic usage with object as underlying data store.
--FILE--
<?php

#[AllowDynamicProperties]
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
} catch (TypeError $e) {
    echo "Exception: " . $e->getMessage() . "\n";
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
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
$original->addedToOriginal = 'added To Original';
var_dump($ao, $original, $copy);

?>
--EXPECTF--
--> exchangeArray() with objects:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#3 (2) {
    ["pub1"]=>
    string(7) "public1"
    ["addedToSwapIn"]=>
    string(16) "added To Swap-In"
  }
}
object(C)#1 (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
object(C)#3 (2) {
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
Exception: ArrayObject::exchangeArray() expects exactly 1 argument, 0 given

Warning: Undefined variable $copy in %s on line %d
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#3 (2) {
    ["pub1"]=>
    string(7) "public1"
    ["addedToOriginal"]=>
    string(17) "added To Original"
  }
}
object(C)#3 (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
NULL


--> exchangeArray() with bad arg type:
ArrayObject::exchangeArray(): Argument #1 ($array) must be of type array, null given

Warning: Undefined variable $copy in %s on line %d
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  object(C)#2 (2) {
    ["pub1"]=>
    string(7) "public1"
    ["addedToOriginal"]=>
    string(17) "added To Original"
  }
}
object(C)#2 (2) {
  ["pub1"]=>
  string(7) "public1"
  ["addedToOriginal"]=>
  string(17) "added To Original"
}
NULL
