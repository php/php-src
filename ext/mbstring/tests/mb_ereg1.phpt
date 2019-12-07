--TEST--
mb_ereg() and invalid arguments
--SKIPIF--
<?php if (!function_exists("mb_ereg")) print "skip"; ?>
--FILE--
<?php

$a = array(
	array(1,2,3),
	array("", "", ""),
	array(array(), 1, ""),
	array(1, array(), ""),
	array(1, "", array()),
	);

foreach ($a as $args) {
	try {
		var_dump(mb_ereg($args[0], $args[1], $args[2]));
	} catch (\TypeError|\ValueError $e) {
		echo get_class($e) . ': ' . $e->getMessage(), "\n";
	}
	var_dump($args);
}
?>
--EXPECT--
bool(false)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  array(0) {
  }
}
ValueError: Empty pattern
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  &array(0) {
  }
}
TypeError: mb_ereg() expects argument #1 ($pattern) to be of type string, array given
array(3) {
  [0]=>
  array(0) {
  }
  [1]=>
  int(1)
  [2]=>
  &string(0) ""
}
TypeError: mb_ereg() expects argument #2 ($string) to be of type string, array given
array(3) {
  [0]=>
  int(1)
  [1]=>
  array(0) {
  }
  [2]=>
  &string(0) ""
}
bool(false)
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(0) ""
  [2]=>
  array(0) {
  }
}
