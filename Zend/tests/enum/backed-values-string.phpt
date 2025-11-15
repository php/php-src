--TEST--
Backed enums: values() returns strings
--FILE--
<?php

enum Color: string {
    case Red = 'red';
    case Green = 'green';
    case Blue = 'blue';
}

var_dump(Color::values());

?>
--EXPECT--
array(3) {
  [0]=>
  string(3) "red"
  [1]=>
  string(5) "green"
  [2]=>
  string(4) "blue"
}

