--TEST--
Test asort() function : SORT_STRICT flag
--FILE--
<?php
/*
 * Testing asort() by providing object/array values to check the
 * "strict equality" sort order (SORT_STRICT).
 */

echo "*** Testing asort() : SORT_STRICT flag ***\n";

$arr1 = [ 'foo' => 'bar' ];
$arr2 = [ 'foo' => 'bar' ]; // Same contents as $arr1
$arr3 = [ 'foo' => 'bat' ]; // Different value
$arr4 = [ 'bar' => 'bar' ]; // Different key

echo "\n";
echo "\$arr1 and \$arr2 are value objects which are indistiguishable in PHP\n";
var_dump($arr1 === $arr2); // expect : bool(true)

// Object-type equivalents of the array cases
$obj1 = (object)$arr1;
$obj2 = (object)$arr2; # Unlike $arr1/$arr2, $obj1 !== $obj2
$obj3 = (object)$arr3;
$obj4 = (object)$arr4;

echo "Unlike \$arr1/\$arr2, object values are distinguishable.\n";
var_dump($obj1 === $obj2); // expect : bool(false)

// The object IDs of $obj1 and $obj2 may vary non-deterministically from
// run to run.  But in order to ensure deterministic results from this test,
// we're going to check spl_object_id() and swap $obj1 and $obj2 so that
// $obj1 is guaranteed to have the lower spl_object_id().
if (spl_object_id($obj1) > spl_object_id($obj2)) {
	$tmp = $obj1;
	$obj1 = $obj2;
	$obj2 = $tmp;
}

// Arrays containing values which are == but not ===
$arr5 = [ $arr1 ];
$arr6 = [ $arr2 ];
$arr7 = [ $obj1 ];
$arr8 = [ $obj2 ];

echo "Arrays with indistinguishable objects are themselves indistinguishable.\n";
var_dump($arr5 === $arr6); // expect : bool(true)
echo "But arrays with distinguishable arrays are distinguishable.\n";
var_dump($arr7 === $arr8); // expect : bool(false)

// Object-type equivalents.
$obj5 = (object)[ 'p' => $arr1 ];
$obj6 = (object)[ 'p' => $arr2 ];
$obj7 = (object)[ 'p' => $obj1 ];
$obj8 = (object)[ 'p' => $obj2 ];

echo "These are all distinguishable:\n";
var_dump($obj5 === $obj6); // expect : bool(false)
var_dump($obj7 === $obj8); // expect : bool(false)

echo "\n";
echo "Now, let's sort these!\n";
$pi = 3.141592654;
$temp_array = [
	// We expect all object type values to be last in the sorted result
	'obj1' => $obj1, 'obj2' => $obj2, 'obj3' => $obj3, 'obj4' => $obj4,
	'obj5' => $obj5, 'obj6' => $obj6, 'obj7' => $obj7, 'obj8' => $obj8,
	// We expect all array type values to precede them
	'arr1' => $arr1, 'arr2' => $arr2, 'arr3' => $arr3, 'arr4' => $arr4,
	'arr5' => $arr5, 'arr6' => $arr6, 'arr7' => $arr7, 'arr8' => $arr8,
	// We expect these non-array values to be sorted to the front.
	'bool(true)' => true,
	'null' => null,
	'bool(false)' => false,
	'int(0)' => 0,
	'string(a)' => "a",
	'int(1)' => 1,
	'string(b)' => "b",
	'float' => &$pi,
	'callable' => fn($x) => $x + 1,
];
var_dump( asort($temp_array, SORT_STRICT) ); // expect : bool(true)
// Usually, we'd expect the order of $obj1/$obj2, $arr7/$arr8, $obj5/$obj6
// and $obj7/$obj8 to vary because they depend on object_id.  But we've
// arranged for $arr1/$obj1 to have the lower ID so we can test positions
// precisely.
// (Every other element, except for these six pairs, would always have
// a deterministic position; when PHP8 made the sort stable this
// ensured $arr1/$arr2 and $arr5/$arr6 would have a deterministic order.)
var_dump( $temp_array );

echo "Done\n";
?>
--EXPECTF--
*** Testing asort() : SORT_STRICT flag ***

$arr1 and $arr2 are value objects which are indistiguishable in PHP
bool(true)
Unlike $arr1/$arr2, object values are distinguishable.
bool(false)
Arrays with indistinguishable objects are themselves indistinguishable.
bool(true)
But arrays with distinguishable arrays are distinguishable.
bool(false)
These are all distinguishable:
bool(false)
bool(false)

Now, let's sort these!
bool(true)
array(25) {
  ["null"]=>
  NULL
  ["bool(false)"]=>
  bool(false)
  ["bool(true)"]=>
  bool(true)
  ["int(0)"]=>
  int(0)
  ["int(1)"]=>
  int(1)
  ["float"]=>
  &float(3.141592654)
  ["string(a)"]=>
  string(1) "a"
  ["string(b)"]=>
  string(1) "b"
  ["arr5"]=>
  array(1) {
    [0]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["arr6"]=>
  array(1) {
    [0]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["arr7"]=>
  array(1) {
    [0]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["arr8"]=>
  array(1) {
    [0]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["arr4"]=>
  array(1) {
    ["bar"]=>
    string(3) "bar"
  }
  ["arr1"]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
  ["arr2"]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
  ["arr3"]=>
  array(1) {
    ["foo"]=>
    string(3) "bat"
  }
  ["callable"]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$x"]=>
      string(10) "<required>"
    }
  }
  ["obj5"]=>
  object(stdClass)#%d (1) {
    ["p"]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["obj6"]=>
  object(stdClass)#%d (1) {
    ["p"]=>
    array(1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["obj7"]=>
  object(stdClass)#%d (1) {
    ["p"]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["obj8"]=>
  object(stdClass)#%d (1) {
    ["p"]=>
    object(stdClass)#%d (1) {
      ["foo"]=>
      string(3) "bar"
    }
  }
  ["obj4"]=>
  object(stdClass)#%d (1) {
    ["bar"]=>
    string(3) "bar"
  }
  ["obj1"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(3) "bar"
  }
  ["obj2"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(3) "bar"
  }
  ["obj3"]=>
  object(stdClass)#%d (1) {
    ["foo"]=>
    string(3) "bat"
  }
}
Done
