--TEST--
Test array_column() function: basic functionality
--FILE--
<?php
class ColumnKeyClass {
    function __toString() { return 'first_name'; }
}

class ColumnKeyClass2 {
    function __toString() { return 'last_name'; }
}

class IndexKeyClass {
    function __toString() { return 'id'; }
}

class ValueClass {
    function __toString() { return '2135'; }
}


$column_key = new ColumnKeyClass();
$column_key2 = new ColumnKeyClass2();
$index_key = new IndexKeyClass();
$value = new ValueClass();


// Array representing a possible record set returned from a database
$records = array(
    array(
        'id' => $value,
        'first_name' => 'John',
        'last_name' => 'XXX'
    ),
    array(
        'id' => 3245,
        'first_name' => 'Sally',
        'last_name' => 'Smith'
    ),
);
$firstNames = array_column($records, $column_key, $index_key);
print_r($firstNames);
$firstLastNames = array_column($records, array($column_key, $column_key2));
print_r($firstLastNames);
var_dump($column_key);
var_dump($index_key);
var_dump($value);
--EXPECTF--
Array
(
    [2135] => John
    [3245] => Sally
)
Array
(
    [0] => Array
        (
            [first_name] => John
            [last_name] => XXX
        )

    [1] => Array
        (
            [first_name] => Sally
            [last_name] => Smith
        )

)
object(ColumnKeyClass)#%d (0) {
}
object(IndexKeyClass)#%d (0) {
}
object(ValueClass)#%d (0) {
}
