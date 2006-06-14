--TEST--
x (PCRE_EXTENDED) modififer
--FILE--
<?php

var_dump(preg_match('/a e i o u/', 'aeiou', $m));
var_dump($m);

var_dump(preg_match('/a e i o u/x', 'aeiou', $m));
var_dump($m);

var_dump(preg_match("/a e\ni\to\ru/x", 'aeiou', $m));
var_dump($m);

?>
--EXPECT--
int(0)
array(0) {
}
int(1)
array(1) {
  [0]=>
  string(5) "aeiou"
}
int(1)
array(1) {
  [0]=>
  string(5) "aeiou"
}
