--TEST--
Testing func_get_named_args() with spread operator
--FILE--
<?php

function hello_named(string $name, int $age, ...$args): array {
    return func_get_named_args();
}

var_dump(hello_named(...[
    'age' => 30,
    'name' => 'John',
    'extra' => 'data',
]));

var_dump(hello_named(name: 'John', age: 30, extra: 'data'));

var_dump(hello_named(...[
    'name' => 'Bob',
    'age' => 35,
    'occupation' => 'Developer',
]));

?>
--EXPECT--
array(3) {
  ["name"]=>
  string(4) "John"
  ["age"]=>
  int(30)
  ["extra"]=>
  string(4) "data"
}
array(3) {
  ["name"]=>
  string(4) "John"
  ["age"]=>
  int(30)
  ["extra"]=>
  string(4) "data"
}
array(3) {
  ["name"]=>
  string(3) "Bob"
  ["age"]=>
  int(35)
  ["occupation"]=>
  string(9) "Developer"
}
