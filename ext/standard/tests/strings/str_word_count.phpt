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
var_dump(str_word_count($str, 99999999999999999));
var_dump(str_word_count($str, array()));
var_dump(str_word_count($str, $b));
var_dump($str);

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

Warning: str_word_count(): The specified format parameter, '3' is invalid. in %s on line 9
bool(false)

Warning: str_word_count(): The specified format parameter, '123' is invalid. in %s on line 10
bool(false)

Warning: str_word_count(): The specified format parameter, '-1' is invalid. in %s on line 11
bool(false)

Warning: str_word_count(): The specified format parameter, '1569325056' is invalid. in %s on line 12
bool(false)

Warning: str_word_count(): The specified format parameter, '0' is invalid. in %s on line 13
bool(false)

Warning: str_word_count(): The specified format parameter, '0' is invalid. in %s on line 14
bool(false)
string(55) "Hello friend, you're  
    looking          good today!"