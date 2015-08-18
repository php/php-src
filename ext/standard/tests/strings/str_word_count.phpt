--TEST--
str_word_count()
--FILE--
<?php
error_reporting(E_ALL);
$str = "Hello friend, you're  
    looking          good today!";
$b =& $str;       
var_dump(str_word_count($str, 1));
var_dump(str_word_count($str, 2));
var_dump(str_word_count($str));
var_dump(str_word_count($str, 3)); 
var_dump(str_word_count($str, 123));
var_dump(str_word_count($str, -1));
var_dump(str_word_count($str, 999999999));
var_dump(str_word_count($str, array()));
var_dump(str_word_count($str, $b));
var_dump($str);

$str2 = "F0o B4r 1s bar foo";
var_dump(str_word_count($str2, NULL, "04"));
var_dump(str_word_count($str2, NULL, "01"));
var_dump(str_word_count($str2, NULL, "014"));
var_dump(str_word_count($str2, NULL, array()));
var_dump(str_word_count($str2, NULL, new stdClass));
var_dump(str_word_count($str2, NULL, ""));
var_dump(str_word_count($str2, 1, "04"));
var_dump(str_word_count($str2, 1, "01"));
var_dump(str_word_count($str2, 1, "014"));
var_dump(str_word_count($str2, 1, array()));
var_dump(str_word_count($str2, 1, new stdClass));
var_dump(str_word_count($str2, 1, ""));
var_dump(str_word_count($str2, 2, "04"));
var_dump(str_word_count($str2, 2, "01"));
var_dump(str_word_count($str2, 2, "014"));
var_dump(str_word_count($str2, 2, array()));
var_dump(str_word_count($str2, 2, new stdClass));
var_dump(str_word_count($str2, 2, ""));
var_dump(str_word_count("foo'0 bar-0var", 2, "0"));
var_dump(str_word_count("'foo'", 2));
var_dump(str_word_count("'foo'", 2, "'"));
var_dump(str_word_count("-foo-", 2));
var_dump(str_word_count("-foo-", 2, "-"));

echo "Done\n";
?>
--EXPECTF--
array(6) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(6) "friend"
  [2]=>
  string(6) "you're"
  [3]=>
  string(7) "looking"
  [4]=>
  string(4) "good"
  [5]=>
  string(5) "today"
}
array(6) {
  [0]=>
  string(5) "Hello"
  [6]=>
  string(6) "friend"
  [14]=>
  string(6) "you're"
  [27]=>
  string(7) "looking"
  [44]=>
  string(4) "good"
  [49]=>
  string(5) "today"
}
int(6)

Warning: str_word_count(): Invalid format value 3 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value 123 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value -1 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value 999999999 in %s on line %d
bool(false)

Warning: str_word_count() expects parameter 2 to be integer, array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 2 to be integer, string given in %s on line %d
NULL
string(55) "Hello friend, you're  
    looking          good today!"
int(5)
int(6)
int(5)

Warning: str_word_count() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string, object given in %s on line %d
NULL
int(7)
array(5) {
  [0]=>
  string(3) "F0o"
  [1]=>
  string(3) "B4r"
  [2]=>
  string(1) "s"
  [3]=>
  string(3) "bar"
  [4]=>
  string(3) "foo"
}
array(6) {
  [0]=>
  string(3) "F0o"
  [1]=>
  string(1) "B"
  [2]=>
  string(1) "r"
  [3]=>
  string(2) "1s"
  [4]=>
  string(3) "bar"
  [5]=>
  string(3) "foo"
}
array(5) {
  [0]=>
  string(3) "F0o"
  [1]=>
  string(3) "B4r"
  [2]=>
  string(2) "1s"
  [3]=>
  string(3) "bar"
  [4]=>
  string(3) "foo"
}

Warning: str_word_count() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string, object given in %s on line %d
NULL
array(7) {
  [0]=>
  string(1) "F"
  [1]=>
  string(1) "o"
  [2]=>
  string(1) "B"
  [3]=>
  string(1) "r"
  [4]=>
  string(1) "s"
  [5]=>
  string(3) "bar"
  [6]=>
  string(3) "foo"
}
array(5) {
  [0]=>
  string(3) "F0o"
  [4]=>
  string(3) "B4r"
  [9]=>
  string(1) "s"
  [11]=>
  string(3) "bar"
  [15]=>
  string(3) "foo"
}
array(6) {
  [0]=>
  string(3) "F0o"
  [4]=>
  string(1) "B"
  [6]=>
  string(1) "r"
  [8]=>
  string(2) "1s"
  [11]=>
  string(3) "bar"
  [15]=>
  string(3) "foo"
}
array(5) {
  [0]=>
  string(3) "F0o"
  [4]=>
  string(3) "B4r"
  [8]=>
  string(2) "1s"
  [11]=>
  string(3) "bar"
  [15]=>
  string(3) "foo"
}

Warning: str_word_count() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string, object given in %s on line %d
NULL
array(7) {
  [0]=>
  string(1) "F"
  [2]=>
  string(1) "o"
  [4]=>
  string(1) "B"
  [6]=>
  string(1) "r"
  [9]=>
  string(1) "s"
  [11]=>
  string(3) "bar"
  [15]=>
  string(3) "foo"
}
array(2) {
  [0]=>
  string(5) "foo'0"
  [6]=>
  string(8) "bar-0var"
}
array(1) {
  [1]=>
  string(4) "foo'"
}
array(1) {
  [0]=>
  string(5) "'foo'"
}
array(1) {
  [1]=>
  string(3) "foo"
}
array(1) {
  [0]=>
  string(5) "-foo-"
}
Done
