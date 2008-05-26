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
  unicode(1) "a"
  [1]=>
  unicode(1) "b"
  [2]=>
  unicode(1) "c"
  [3]=>
  unicode(1) "d"
  [4]=>
  unicode(1) "e"
  [5]=>
  unicode(1) "f"
  [6]=>
  unicode(1) "g"
  [7]=>
  unicode(1) "h"
  [8]=>
  unicode(1) "i"
  [9]=>
  unicode(1) "j"
  [10]=>
  unicode(1) "k"
  [11]=>
  unicode(1) "l"
  [12]=>
  unicode(1) "m"
  [13]=>
  unicode(1) "n"
  [14]=>
  unicode(1) "o"
  [15]=>
  unicode(1) "p"
  [16]=>
  unicode(1) "q"
  [17]=>
  unicode(1) "r"
  [18]=>
  unicode(1) "s"
  [19]=>
  unicode(1) "t"
  [20]=>
  unicode(1) "u"
  [21]=>
  unicode(1) "v"
  [22]=>
  unicode(1) "w"
  [23]=>
  unicode(1) "x"
  [24]=>
  unicode(1) "y"
  [25]=>
  unicode(1) "z"
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
