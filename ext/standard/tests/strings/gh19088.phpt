--TEST--
GH-19088 (sscanf %c conversion should read whitespace)
--FILE--
<?php

$string = '     ';

$result = sscanf($string, '%c%n', $char, $offset);
var_dump($result, $char, $offset);

var_dump(sscanf($string, '%3c%n'));

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
