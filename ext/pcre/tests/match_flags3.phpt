--TEST--
preg_match() flags 3
--FILE--
<?php

var_dump(preg_match('', '', $match, 0xfff));

var_dump(preg_match('/\d+/', '123 456 789 012', $match, 0, -8));
var_dump($match);

var_dump(preg_match('/\d+/', '123 456 789 012', $match, 0, -500));
var_dump($match);

var_dump(preg_match_all('/\d+/', '123 456 789 012', $match, 0, -8));
var_dump($match);

var_dump(preg_match('/(?P<3>)/', ''));

?>
--EXPECTF--

Warning: preg_match(): Empty regular expression in %smatch_flags3.php on line 3
bool(false)
int(1)
array(1) {
  [0]=>
  string(3) "789"
}
int(1)
array(1) {
  [0]=>
  string(3) "123"
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

Warning: preg_match(): Compilation failed: group name must start with a non-digit at offset %d in %smatch_flags3.php on line %d
bool(false)
