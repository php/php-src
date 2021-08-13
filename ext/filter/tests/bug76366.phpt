--TEST--
Bug #76366 (references in sub-array for filtering breaks the filter)
--EXTENSIONS--
filter
--FILE--
<?php

#array to filter
$data = ['foo' => 6];

#filter args
$args = [
    'foo'=> [
        'filter' => FILTER_VALIDATE_INT,
        'flags' => FILTER_FORCE_ARRAY
    ]
];

$args['foo']['options'] = [];

#create reference
$options = &$args['foo']['options'];

#set options
$options['min_range'] = 1;
$options['max_range'] = 5;

#show the filter result
var_dump(filter_var_array($data, $args));
?>
--EXPECT--
array(1) {
  ["foo"]=>
  array(1) {
    [0]=>
    bool(false)
  }
}
