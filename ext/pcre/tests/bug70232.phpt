--TEST--
Bug #70232 (Incorrect bump-along behavior with \K and empty string match)
--SKIPIF--
<?php
if (version_compare(explode(' ', PCRE_VERSION)[0], '8.0', 'lt')) {
    die("skip this test requires libpcre >= 8.0");
}
?>
--FILE--
<?php
$pattern = '~(?: |\G)\d\B\K~';
$subject = "123 a123 1234567 b123 123";
preg_match_all($pattern, $subject, $matches);
var_dump($matches);
var_dump(preg_replace($pattern, "*", $subject));
var_dump(preg_split($pattern, $subject));
?>
--EXPECT--
array(1) {
  [0]=>
  array(10) {
    [0]=>
    string(0) ""
    [1]=>
    string(0) ""
    [2]=>
    string(0) ""
    [3]=>
    string(0) ""
    [4]=>
    string(0) ""
    [5]=>
    string(0) ""
    [6]=>
    string(0) ""
    [7]=>
    string(0) ""
    [8]=>
    string(0) ""
    [9]=>
    string(0) ""
  }
}
string(35) "1*2*3 a123 1*2*3*4*5*6*7 b123 1*2*3"
array(11) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(8) "3 a123 1"
  [3]=>
  string(1) "2"
  [4]=>
  string(1) "3"
  [5]=>
  string(1) "4"
  [6]=>
  string(1) "5"
  [7]=>
  string(1) "6"
  [8]=>
  string(8) "7 b123 1"
  [9]=>
  string(1) "2"
  [10]=>
  string(1) "3"
}
