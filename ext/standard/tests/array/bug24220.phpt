--TEST--n
Bug #24220 (range() numeric string handling)
--FILE--
<?php
	var_dump(range("2003", "2004"));
	var_dump(range("a", "z"));
	var_dump(range("1", "10"));
?>
--EXPECT--
array(2) {
  [0]=>
  int(2003)
  [1]=>
  int(2004)
}
array(26) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
  [3]=>
  string(1) "d"
  [4]=>
  string(1) "e"
  [5]=>
  string(1) "f"
  [6]=>
  string(1) "g"
  [7]=>
  string(1) "h"
  [8]=>
  string(1) "i"
  [9]=>
  string(1) "j"
  [10]=>
  string(1) "k"
  [11]=>
  string(1) "l"
  [12]=>
  string(1) "m"
  [13]=>
  string(1) "n"
  [14]=>
  string(1) "o"
  [15]=>
  string(1) "p"
  [16]=>
  string(1) "q"
  [17]=>
  string(1) "r"
  [18]=>
  string(1) "s"
  [19]=>
  string(1) "t"
  [20]=>
  string(1) "u"
  [21]=>
  string(1) "v"
  [22]=>
  string(1) "w"
  [23]=>
  string(1) "x"
  [24]=>
  string(1) "y"
  [25]=>
  string(1) "z"
}
array(10) {
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
  [5]=>
  int(6)
  [6]=>
  int(7)
  [7]=>
  int(8)
  [8]=>
  int(9)
  [9]=>
  int(10)
}
