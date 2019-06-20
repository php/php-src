--TEST--
explode() function with empty delimiter
--FILE--
<?php
$string = "Lorem Ipsum\nMuspi Merol";

echo "Empty delimiter without limit argument:\n";
var_dump(explode("", $string));
var_dump(explode(NULL, $string));
var_dump(explode(false, $string));

echo "Empty delimiter with positive limit:\n";
var_dump(explode("", $string, 3));
var_dump(explode(NULL, $string, 3));
var_dump(explode(false, $string, 3));

echo "Empty delimiter with negative limit:\n";
var_dump(explode("", $string, -15));
var_dump(explode(NULL, $string, -15));
var_dump(explode(false, $string, -15));


echo "Empty delimiter with negative limit equal to minus string length:\n";
var_dump(explode("", $string, -strlen($string)));
var_dump(explode(NULL, $string, -strlen($string)));
var_dump(explode(false, $string, -strlen($string)));

echo "Empty delimiter with largest negative limit:\n"; // i.e. smallest system integer
var_dump(explode("", $string, PHP_INT_MIN));
var_dump(explode(NULL, $string, PHP_INT_MIN));
var_dump(explode(false, $string, PHP_INT_MIN));
?>
--EXPECT--
Empty delimiter without limit argument:
array(23) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
  [8]=>
  string(1) "s"
  [9]=>
  string(1) "u"
  [10]=>
  string(1) "m"
  [11]=>
  string(1) "
"
  [12]=>
  string(1) "M"
  [13]=>
  string(1) "u"
  [14]=>
  string(1) "s"
  [15]=>
  string(1) "p"
  [16]=>
  string(1) "i"
  [17]=>
  string(1) " "
  [18]=>
  string(1) "M"
  [19]=>
  string(1) "e"
  [20]=>
  string(1) "r"
  [21]=>
  string(1) "o"
  [22]=>
  string(1) "l"
}
array(23) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
  [8]=>
  string(1) "s"
  [9]=>
  string(1) "u"
  [10]=>
  string(1) "m"
  [11]=>
  string(1) "
"
  [12]=>
  string(1) "M"
  [13]=>
  string(1) "u"
  [14]=>
  string(1) "s"
  [15]=>
  string(1) "p"
  [16]=>
  string(1) "i"
  [17]=>
  string(1) " "
  [18]=>
  string(1) "M"
  [19]=>
  string(1) "e"
  [20]=>
  string(1) "r"
  [21]=>
  string(1) "o"
  [22]=>
  string(1) "l"
}
array(23) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
  [8]=>
  string(1) "s"
  [9]=>
  string(1) "u"
  [10]=>
  string(1) "m"
  [11]=>
  string(1) "
"
  [12]=>
  string(1) "M"
  [13]=>
  string(1) "u"
  [14]=>
  string(1) "s"
  [15]=>
  string(1) "p"
  [16]=>
  string(1) "i"
  [17]=>
  string(1) " "
  [18]=>
  string(1) "M"
  [19]=>
  string(1) "e"
  [20]=>
  string(1) "r"
  [21]=>
  string(1) "o"
  [22]=>
  string(1) "l"
}
Empty delimiter with positive limit:
array(3) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(21) "rem Ipsum
Muspi Merol"
}
array(3) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(21) "rem Ipsum
Muspi Merol"
}
array(3) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(21) "rem Ipsum
Muspi Merol"
}
Empty delimiter with negative limit:
array(8) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
}
array(8) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
}
array(8) {
  [0]=>
  string(1) "L"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "r"
  [3]=>
  string(1) "e"
  [4]=>
  string(1) "m"
  [5]=>
  string(1) " "
  [6]=>
  string(1) "I"
  [7]=>
  string(1) "p"
}
Empty delimiter with negative limit equal to minus string length:
array(0) {
}
array(0) {
}
array(0) {
}
Empty delimiter with largest negative limit:
array(0) {
}
array(0) {
}
array(0) {
}