--TEST--
GH-19088 (sscanf %c conversion should read whitespace)
--FILE--
<?php

$string = '     ';

$result = sscanf($string, '%c%n', $char, $offset);
var_dump($result, $char, $offset);

var_dump(sscanf($string, '%3c%n'));

$result = sscanf('hi world', '%5c', $a);
var_dump($result, $a);

$result = sscanf('a', '%5c', $a);
var_dump($result, $a);

?>
--EXPECT--
int(2)
string(1) " "
int(1)
array(2) {
  [0]=>
  string(3) "   "
  [1]=>
  int(3)
}
int(1)
string(5) "hi wo"
int(1)
string(1) "a"
