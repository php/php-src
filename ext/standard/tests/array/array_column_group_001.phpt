--TEST--
Group the whole array using integer index
--FILE--
<?php
$ar = [
    ['id' => 1, 'name' => 'hassan', 'score' => 'A', 's' => '1'],
    ['id' => 2, 'name' => 'hassan', 'score' => 'B', 's' => '2'],
    ['id' => 3, 'name' => 'ahmed', 'score' => 'C', 's' => '1'],
    ['id' => 4, 'name' => 'moustafa', 'score' => 'D', 's' => '2'],
    ['id' => 5, 'name' => 'hassan', 'score' => 'A', 's' => '3'],
    ['id' => 6, 'name' => 'wael', 'score' => 'A', 's' => '3'],
    ['id' => 6, 'name' => 'wael', 'score' => 'D', 's' => '1'],
];

var_dump(array_column($ar, null, 'id', true));
?>
--EXPECT--
array(6) {
  [1]=>
  array(1) {
    [0]=>
    array(4) {
      ["id"]=>
      int(1)
      ["name"]=>
      string(6) "hassan"
      ["score"]=>
      string(1) "A"
      ["s"]=>
      string(1) "1"
    }
  }
  [2]=>
  array(1) {
    [0]=>
    array(4) {
      ["id"]=>
      int(2)
      ["name"]=>
      string(6) "hassan"
      ["score"]=>
      string(1) "B"
      ["s"]=>
      string(1) "2"
    }
  }
  [3]=>
  array(1) {
    [0]=>
    array(4) {
      ["id"]=>
      int(3)
      ["name"]=>
      string(5) "ahmed"
      ["score"]=>
      string(1) "C"
      ["s"]=>
      string(1) "1"
    }
  }
  [4]=>
  array(1) {
    [0]=>
    array(4) {
      ["id"]=>
      int(4)
      ["name"]=>
      string(8) "moustafa"
      ["score"]=>
      string(1) "D"
      ["s"]=>
      string(1) "2"
    }
  }
  [5]=>
  array(1) {
    [0]=>
    array(4) {
      ["id"]=>
      int(5)
      ["name"]=>
      string(6) "hassan"
      ["score"]=>
      string(1) "A"
      ["s"]=>
      string(1) "3"
    }
  }
  [6]=>
  array(2) {
    [0]=>
    array(4) {
      ["id"]=>
      int(6)
      ["name"]=>
      string(4) "wael"
      ["score"]=>
      string(1) "A"
      ["s"]=>
      string(1) "3"
    }
    [1]=>
    array(4) {
      ["id"]=>
      int(6)
      ["name"]=>
      string(4) "wael"
      ["score"]=>
      string(1) "D"
      ["s"]=>
      string(1) "1"
    }
  }
}