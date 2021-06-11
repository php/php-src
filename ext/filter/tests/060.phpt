--TEST--
filter_var() - tests for the range options of filter FILTER_VALIDATE_FLOAT
--INI--
precision=14
--EXTENSIONS--
filter
--FILE--
<?php

$values = [
    null,
    false,
    0,
    -1,
    '-5.4',
    '-5.5',
    '2,000.00',
    '2,000.01',
    '1,999.9999999'
];

var_dump(filter_var(
    $values,
    FILTER_VALIDATE_FLOAT,
    [
        'options' => [
            'min_range' => -5.4,
            'max_range' => 2000,
        ],
        'flags' => FILTER_FLAG_ALLOW_THOUSAND | FILTER_REQUIRE_ARRAY
    ]
));

var_dump(filter_var(
    '1000',
    FILTER_VALIDATE_FLOAT,
    [
        'options' => [
            'max_range' => 999.999,
            'default' => 0
        ]
    ]
));

var_dump(filter_var(
    '-11',
    FILTER_VALIDATE_FLOAT,
    [
        'options' => [
            'min_range' => -10,
            'default' => 0
        ]
    ]
));
?>
--EXPECT--
array(9) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
  [2]=>
  float(0)
  [3]=>
  float(-1)
  [4]=>
  float(-5.4)
  [5]=>
  bool(false)
  [6]=>
  float(2000)
  [7]=>
  bool(false)
  [8]=>
  float(1999.9999999)
}
int(0)
int(0)
