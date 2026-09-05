--TEST--
Test array_column() - argument types
--FILE--
<?php

foreach ([
    fn() => array_column([['first', 'second']], false),
    fn() => array_column([], []),
    fn() => array_column([['value' => 1, 0 => 'key']], 'value', false),
    fn() => array_column([], null, []),
] as $callback) {
    try {
        var_dump($callback());
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
array(1) {
  [0]=>
  string(5) "first"
}
TypeError: array_column(): Argument #2 ($column_key) must be of type string|int|null, array given
array(1) {
  ["key"]=>
  int(1)
}
TypeError: array_column(): Argument #3 ($index_key) must be of type string|int|null, array given
