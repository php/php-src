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
var_dump(str_word_count($str, 99999999));
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
  unicode(5) "Hello"
  [1]=>
  unicode(6) "friend"
  [2]=>
  unicode(6) "you're"
  [3]=>
  unicode(7) "looking"
  [4]=>
  unicode(4) "good"
  [5]=>
  unicode(5) "today"
}
array(6) {
  [0]=>
  unicode(5) "Hello"
  [6]=>
  unicode(6) "friend"
  [14]=>
  unicode(6) "you're"
  [27]=>
  unicode(7) "looking"
  [44]=>
  unicode(4) "good"
  [49]=>
  unicode(5) "today"
}
int(6)

Warning: str_word_count(): Invalid format value 3 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value 123 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value -1 in %s on line %d
bool(false)

Warning: str_word_count(): Invalid format value 99999999 in %s on line %d
bool(false)

Warning: str_word_count() expects parameter 2 to be long, array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 2 to be long, Unicode string given in %s on line %d
NULL
unicode(55) "Hello friend, you're  
    looking          good today!"
int(5)
int(6)
int(5)

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), object given in %s on line %d
NULL
int(7)
array(5) {
  [0]=>
  unicode(3) "F0o"
  [1]=>
  unicode(3) "B4r"
  [2]=>
  unicode(1) "s"
  [3]=>
  unicode(3) "bar"
  [4]=>
  unicode(3) "foo"
}
array(6) {
  [0]=>
  unicode(3) "F0o"
  [1]=>
  unicode(1) "B"
  [2]=>
  unicode(1) "r"
  [3]=>
  unicode(2) "1s"
  [4]=>
  unicode(3) "bar"
  [5]=>
  unicode(3) "foo"
}
array(5) {
  [0]=>
  unicode(3) "F0o"
  [1]=>
  unicode(3) "B4r"
  [2]=>
  unicode(2) "1s"
  [3]=>
  unicode(3) "bar"
  [4]=>
  unicode(3) "foo"
}

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), object given in %s on line %d
NULL
array(7) {
  [0]=>
  unicode(1) "F"
  [1]=>
  unicode(1) "o"
  [2]=>
  unicode(1) "B"
  [3]=>
  unicode(1) "r"
  [4]=>
  unicode(1) "s"
  [5]=>
  unicode(3) "bar"
  [6]=>
  unicode(3) "foo"
}
array(5) {
  [0]=>
  unicode(3) "F0o"
  [4]=>
  unicode(3) "B4r"
  [9]=>
  unicode(1) "s"
  [11]=>
  unicode(3) "bar"
  [15]=>
  unicode(3) "foo"
}
array(6) {
  [0]=>
  unicode(3) "F0o"
  [4]=>
  unicode(1) "B"
  [6]=>
  unicode(1) "r"
  [8]=>
  unicode(2) "1s"
  [11]=>
  unicode(3) "bar"
  [15]=>
  unicode(3) "foo"
}
array(5) {
  [0]=>
  unicode(3) "F0o"
  [4]=>
  unicode(3) "B4r"
  [8]=>
  unicode(2) "1s"
  [11]=>
  unicode(3) "bar"
  [15]=>
  unicode(3) "foo"
}

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: str_word_count() expects parameter 3 to be string (Unicode or binary), object given in %s on line %d
NULL
array(7) {
  [0]=>
  unicode(1) "F"
  [2]=>
  unicode(1) "o"
  [4]=>
  unicode(1) "B"
  [6]=>
  unicode(1) "r"
  [9]=>
  unicode(1) "s"
  [11]=>
  unicode(3) "bar"
  [15]=>
  unicode(3) "foo"
}
array(2) {
  [0]=>
  unicode(5) "foo'0"
  [6]=>
  unicode(8) "bar-0var"
}
array(1) {
  [1]=>
  unicode(4) "foo'"
}
array(1) {
  [0]=>
  unicode(5) "'foo'"
}
array(1) {
  [1]=>
  unicode(3) "foo"
}
array(1) {
  [0]=>
  unicode(5) "-foo-"
}
Done
