--TEST--
json_encode() with JSON_PRETTY_PRINT with 2 spaces string indentation
--FILE--
<?php

$data = [
    'key' => 4,
    'other_key' => [0, 1, 2, 3]
];

echo json_encode($data, JSON_PRETTY_PRINT, 512, 2);

?>
--EXPECT--
{
  "key": 4,
  "other_key": [
    0,
    1,
    2,
    3
  ]
}

