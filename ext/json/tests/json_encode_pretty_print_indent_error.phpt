--TEST--
json_encode() with JSON_PRETTY_PRINT with negative indentation
--FILE--
<?php

$data = [
    'key' => 4,
    'other_key' => [0, 1, 2, 3]
];

try {
  echo json_encode($data, JSON_PRETTY_PRINT, 512, -2);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing json_encode() : error conditions ***

-- Testing json_encode() function (JSON_PRETTY_PRINT) with negative indentation --
json_encode(): Argument #4 ($indent) must be a number greater than or equal to 0
