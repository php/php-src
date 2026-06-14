--TEST--
Test var_dump() function with booleans
--FILE--
<?php

$values = [
  'true' => true,
  'false' => false,
];

foreach ($values as $key => $value) {
    echo "$key:\n";
    var_dump($value);
}

?>
DONE
--EXPECT--
true:
bool(true)
false:
bool(false)
DONE
