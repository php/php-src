--TEST--
Bug #79868: Sorting with array_unique gives unwanted result
--FILE--
<?php

var_dump(array_unique(['b', 'a', 'b'], SORT_REGULAR));

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "a"
}
