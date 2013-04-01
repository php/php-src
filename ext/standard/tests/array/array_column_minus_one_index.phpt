--TEST--
Test array_column() function: basic functionality (saving of -1 index)
--FILE--
<?php
$records = array(
    array(
        'id' => 2135,
        'first_name' => 'John',
        'last_name' => 'Doe'
    ),
    array(
        'id' => -1,
        'first_name' => 'Sally',
        'last_name' => 'Smith'
    ),
    array(
        'id' => -2,
        'first_name' => 'Jane',
        'last_name' => 'Jones'
    ),
);
 
$firstNames = array_column($records, 'first_name', 'id');
print_r($firstNames);
?>
--EXPECTF--
Array
(
    [2135] => John
    [-1] => Sally
    [-2] => Jane
)

