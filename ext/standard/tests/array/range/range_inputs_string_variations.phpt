--TEST--
Test range() function with unexpected string input variations or unusual step.
--INI--
serialize_precision=14
--FILE--
<?php

echo "int compatible float as step\n";
var_dump( range("a", "h", 2.0) );

// What range does is increment the ASCII character code point
echo "A to z range()\n";
var_dump( range("A", "z") );

echo "Done\n";
?>
--EXPECT--
int compatible float as step
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "c"
  [2]=>
  string(1) "e"
  [3]=>
  string(1) "g"
}
A to z range()
array(58) {
  [0]=>
  string(1) "A"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "C"
  [3]=>
  string(1) "D"
  [4]=>
  string(1) "E"
  [5]=>
  string(1) "F"
  [6]=>
  string(1) "G"
  [7]=>
  string(1) "H"
  [8]=>
  string(1) "I"
  [9]=>
  string(1) "J"
  [10]=>
  string(1) "K"
  [11]=>
  string(1) "L"
  [12]=>
  string(1) "M"
  [13]=>
  string(1) "N"
  [14]=>
  string(1) "O"
  [15]=>
  string(1) "P"
  [16]=>
  string(1) "Q"
  [17]=>
  string(1) "R"
  [18]=>
  string(1) "S"
  [19]=>
  string(1) "T"
  [20]=>
  string(1) "U"
  [21]=>
  string(1) "V"
  [22]=>
  string(1) "W"
  [23]=>
  string(1) "X"
  [24]=>
  string(1) "Y"
  [25]=>
  string(1) "Z"
  [26]=>
  string(1) "["
  [27]=>
  string(1) "\"
  [28]=>
  string(1) "]"
  [29]=>
  string(1) "^"
  [30]=>
  string(1) "_"
  [31]=>
  string(1) "`"
  [32]=>
  string(1) "a"
  [33]=>
  string(1) "b"
  [34]=>
  string(1) "c"
  [35]=>
  string(1) "d"
  [36]=>
  string(1) "e"
  [37]=>
  string(1) "f"
  [38]=>
  string(1) "g"
  [39]=>
  string(1) "h"
  [40]=>
  string(1) "i"
  [41]=>
  string(1) "j"
  [42]=>
  string(1) "k"
  [43]=>
  string(1) "l"
  [44]=>
  string(1) "m"
  [45]=>
  string(1) "n"
  [46]=>
  string(1) "o"
  [47]=>
  string(1) "p"
  [48]=>
  string(1) "q"
  [49]=>
  string(1) "r"
  [50]=>
  string(1) "s"
  [51]=>
  string(1) "t"
  [52]=>
  string(1) "u"
  [53]=>
  string(1) "v"
  [54]=>
  string(1) "w"
  [55]=>
  string(1) "x"
  [56]=>
  string(1) "y"
  [57]=>
  string(1) "z"
}
Done
