--TEST--
array_change_key_case()
--FILE--
<?php
$arrays = array (
	array (),
	array (0),
	array (1),
	array (-1),
	array (0, 2, 3, 4, 5),
	array (1, 2, 3, 4, 5),
	array ("" => 1),
	array ("a" => 1),
	array ("Z" => 1),
	array ("one" => 1),
	array ("ONE" => 1),
	array ("OnE" => 1),
	array ("oNe" => 1),
	array ("one" => 1, "two" => 2),
	array ("ONE" => 1, "two" => 2),
	array ("OnE" => 1, "two" => 2),
	array ("oNe" => 1, "two" => 2),
	array ("one" => 1, "TWO" => 2),
	array ("ONE" => 1, "TWO" => 2),
	array ("OnE" => 1, "TWO" => 2),
	array ("oNe" => 1, "TWO" => 2),
	array ("one" => 1, "TwO" => 2),
	array ("ONE" => 1, "TwO" => 2),
	array ("OnE" => 1, "TwO" => 2),
	array ("oNe" => 1, "TwO" => 2),
	array ("one" => 1, "tWo" => 2),
	array ("ONE" => 1, "tWo" => 2),
	array ("OnE" => 1, "tWo" => 2),
	array ("oNe" => 1, "tWo" => 2),
	array ("one" => 1, 2),
	array ("ONE" => 1, 2),
	array ("OnE" => 1, 2),
	array ("oNe" => 1, 2),
	array ("ONE" => 1, "TWO" => 2, "THREE" => 3, "FOUR" => "four"),
	array ("one" => 1, "two" => 2, "three" => 3, "four" => "FOUR"),
	array ("ONE" => 1, "TWO" => 2, "three" => 3, "four" => "FOUR"),
	array ("one" => 1, "two" => 2, "THREE" => 3, "FOUR" => "four")
);

foreach ($arrays as $item) {
	var_dump(array_change_key_case($item));
	var_dump(array_change_key_case($item, CASE_UPPER));
	var_dump(array_change_key_case($item, CASE_LOWER));
	echo "\n";
}
echo "end\n";
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}

array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(0)
}

array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}

array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-1)
}
array(1) {
  [0]=>
  int(-1)
}

array(5) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}

array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}

array(1) {
  [""]=>
  int(1)
}
array(1) {
  [""]=>
  int(1)
}
array(1) {
  [""]=>
  int(1)
}

array(1) {
  ["a"]=>
  int(1)
}
array(1) {
  ["A"]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(1)
}

array(1) {
  ["z"]=>
  int(1)
}
array(1) {
  ["Z"]=>
  int(1)
}
array(1) {
  ["z"]=>
  int(1)
}

array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["ONE"]=>
  int(1)
}
array(1) {
  ["one"]=>
  int(1)
}

array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["ONE"]=>
  int(1)
}
array(1) {
  ["one"]=>
  int(1)
}

array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["ONE"]=>
  int(1)
}
array(1) {
  ["one"]=>
  int(1)
}

array(1) {
  ["one"]=>
  int(1)
}
array(1) {
  ["ONE"]=>
  int(1)
}
array(1) {
  ["one"]=>
  int(1)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}

array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["ONE"]=>
  int(1)
  [0]=>
  int(2)
}
array(2) {
  ["one"]=>
  int(1)
  [0]=>
  int(2)
}

array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "four"
}
array(4) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
  ["FOUR"]=>
  string(4) "four"
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "four"
}

array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "FOUR"
}
array(4) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
  ["FOUR"]=>
  string(4) "FOUR"
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "FOUR"
}

array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "FOUR"
}
array(4) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
  ["FOUR"]=>
  string(4) "FOUR"
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "FOUR"
}

array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "four"
}
array(4) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
  ["FOUR"]=>
  string(4) "four"
}
array(4) {
  ["one"]=>
  int(1)
  ["two"]=>
  int(2)
  ["three"]=>
  int(3)
  ["four"]=>
  string(4) "four"
}

end
