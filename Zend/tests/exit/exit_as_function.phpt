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
--EXPECTF--
string(4) "exit"
string(3) "die"
object(Closure)#%d (2) {
  ["function"]=>
  string(4) "exit"
  ["parameter"]=>
  array(1) {
    ["$status"]=>
    string(10) "<optional>"
  }
}
object(Closure)#%d (2) {
  ["function"]=>
  string(4) "exit"
  ["parameter"]=>
  array(1) {
    ["$status"]=>
    string(10) "<optional>"
  }
}
