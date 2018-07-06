--TEST--
Bug #76366 (references in sub-array for filtering breaks the filter)
--SKIPIF--
<?php
if (!extension_loaded('filter')) die('skip filter extension not available');
?>
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
print_r(filter_var_array($data, $args));
?>
===DONE===
--EXPECT--
Array
(
    [foo] => Array
        (
            [0] =>
        )

)
===DONE===
