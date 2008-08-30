--TEST--
Test array_replace and array_replace_recursive
--FILE--
<?php

$array1 = array(
	0 => 'dontclobber',
	'1' => 'unclobbered',
	'test2' => 0.0,
	'test3' => array(
		'testarray2' => true,
		1 => array(
			'testsubarray1' => 'dontclobber2',
			'testsubarray2' => 'dontclobber3',
	),
    ),
);

$array2 = array(
	1 => 'clobbered',
	'test3' => array(
		'testarray2' => false,
	),
	'test4' => array(
		'clobbered3' => array(0, 1, 2),
	),
);

$array3 = array(array(array(array())));

$array4 = array();
$array4[] = &$array4;

echo " -- Testing array_replace() --\n";
$data = array_replace($array1, $array2);

var_dump($data);

echo " -- Testing array_replace_recursive() --\n";
$data = array_replace_recursive($array1, $array2);

var_dump($data);

echo " -- Testing array_replace_recursive() w/ endless recusrsion --\n";
$data = array_replace_recursive($array3, $array4);

var_dump($data);
?>
--EXPECTF--
 -- Testing array_replace() --
array(5) {
  [0]=>
  string(11) "dontclobber"
  [1]=>
  string(9) "clobbered"
  ["test2"]=>
  float(0)
  ["test3"]=>
  array(1) {
    ["testarray2"]=>
    bool(false)
  }
  ["test4"]=>
  array(1) {
    ["clobbered3"]=>
    array(3) {
      [0]=>
      int(0)
      [1]=>
      int(1)
      [2]=>
      int(2)
    }
  }
}
 -- Testing array_replace_recursive() --
array(5) {
  [0]=>
  string(11) "dontclobber"
  [1]=>
  string(9) "clobbered"
  ["test2"]=>
  float(0)
  ["test3"]=>
  array(2) {
    ["testarray2"]=>
    bool(false)
    [1]=>
    array(2) {
      ["testsubarray1"]=>
      string(12) "dontclobber2"
      ["testsubarray2"]=>
      string(12) "dontclobber3"
    }
  }
  ["test4"]=>
  array(1) {
    ["clobbered3"]=>
    array(3) {
      [0]=>
      int(0)
      [1]=>
      int(1)
      [2]=>
      int(2)
    }
  }
}
 -- Testing array_replace_recursive() w/ endless recusrsion --

Warning: array_replace_recursive(): recursion detected in %s on line %d
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
}
