--TEST--
exit() as function
--FILE--
<?php

function foo(callable $fn) {
    var_dump($fn);
}

$values = [
    'exit',
    'die',
    exit(...),
    die(...),
];

foreach ($values as $value) {
    foo($value);
}

?>
--EXPECT--
string(4) "exit"
string(3) "die"
object(Closure)#1 (2) {
  ["function"]=>
  string(4) "exit"
  ["parameter"]=>
  array(1) {
    ["$code"]=>
    string(10) "<optional>"
  }
}
object(Closure)#2 (2) {
  ["function"]=>
  string(3) "die"
  ["parameter"]=>
  array(1) {
    ["$code"]=>
    string(10) "<optional>"
  }
}
