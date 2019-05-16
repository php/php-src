--TEST--
Bug #41686 (Omitting length param in array_slice not possible)
--FILE--
<?php
$a = array(1,2,3);
$b = array('a'=>1,'b'=>1,'c'=>2);

var_dump(
		array_slice($a, 1),
		array_slice($a, 1, 2, TRUE),
		array_slice($a, 1, NULL, TRUE),
		array_slice($b, 1),
		array_slice($b, 1, 2, TRUE),
		array_slice($b, 1, NULL, TRUE)
);

echo "Done\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
array(2) {
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(2) {
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
}
array(2) {
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
}
array(2) {
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
}
Done
