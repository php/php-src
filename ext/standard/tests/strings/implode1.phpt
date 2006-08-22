--TEST--
implode() and various args 
--FILE--
<?php

$a = array(
	array(1,2),
	array(1.1,2.2),
	array(array(2),array(1)),
	array(false,true),
	);

foreach ($a as $val) {
	var_dump(implode(', ', $val));
	var_dump($val);
}

echo "Done\n";
?>
--EXPECTF--	
string(4) "1, 2"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
string(8) "1.1, 2.2"
array(2) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
}

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
string(12) "Array, Array"
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
string(3) ", 1"
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
Done
--UEXPECTF--
unicode(4) "1, 2"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
unicode(8) "1.1, 2.2"
array(2) {
  [0]=>
  float(1.1)
  [1]=>
  float(2.2)
}

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d
unicode(12) "Array, Array"
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
}
unicode(3) ", 1"
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(true)
}
Done
