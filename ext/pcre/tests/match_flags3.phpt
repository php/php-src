--TEST--
preg_match() flags 3
--FILE--
<?php

var_dump(preg_match('', '', $match, 0xfff));

var_dump(preg_match('/\d+/', '123 456 789 012', $match, 0, -8));
var_dump($match);

var_dump(preg_match_all('/\d+/', '123 456 789 012', $match, 0, -8));
var_dump($match);

var_dump(preg_match('/(?P<3>)/', ''));

?>
--EXPECTF--
Warning: Wrong value for parameter 4 in call to preg_match() in %smatch_flags3.php on line 3
NULL
int(1)
array(1) {
  [0]=>
  string(3) "789"
}
int(2)
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "789"
    [1]=>
    string(3) "012"
  }
}

Warning: preg_match: numeric named subpatterns are not allowed in %smatch_flags3.php on line 11
bool(false)
