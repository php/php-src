--TEST--
BcMath\Number cast
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    '0',
    '0.0',
    '2',
    '1234',
    '12.0004',
    '0.1230',
    1,
    12345,
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);
    var_dump([
        'bool' => (bool) $num,
        'string' => (string) $num,
        'array' => (array) $num,
    ]);
}
?>
--EXPECT--
array(3) {
  ["bool"]=>
  bool(false)
  ["string"]=>
  string(1) "0"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(1) "0"
    ["scale"]=>
    int(0)
  }
}
array(3) {
  ["bool"]=>
  bool(false)
  ["string"]=>
  string(3) "0.0"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(3) "0.0"
    ["scale"]=>
    int(1)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(1) "2"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(1) "2"
    ["scale"]=>
    int(0)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(4) "1234"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(4) "1234"
    ["scale"]=>
    int(0)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(7) "12.0004"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(7) "12.0004"
    ["scale"]=>
    int(4)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(6) "0.1230"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(6) "0.1230"
    ["scale"]=>
    int(4)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(1) "1"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["scale"]=>
    int(0)
  }
}
array(3) {
  ["bool"]=>
  bool(true)
  ["string"]=>
  string(5) "12345"
  ["array"]=>
  array(2) {
    ["value"]=>
    string(5) "12345"
    ["scale"]=>
    int(0)
  }
}
