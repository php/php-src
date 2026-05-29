--TEST--
array_filter() tests with ARRAY_FILTER_PACK_RESULT
--FILE--
<?php

$array1 = [false, null, true, '', 'foo', 0, 123];
var_dump( array_filter( $array1 ) );
var_dump( array_filter( $array1, null, ARRAY_FILTER_PACK_RESULT ) );

$array2 = [
    'index 0 means false key',
    '' => 'false key empty string',
    false,
];
// to filter based on a truthy key requires an explicit callback
var_dump( array_filter( $array2, fn ($v) => $v, ARRAY_FILTER_USE_KEY ) );
var_dump( array_filter( $array2, fn ($v) => $v, ARRAY_FILTER_USE_KEY | ARRAY_FILTER_PACK_RESULT ) );

$array3 = [
    'key1' => true,
    'key2' => false,
    'key3' => 123,
];
function filter( $value, $key ) {
    $value = var_export( $value, true );
    echo 'Filter: $key => $value\n';
    return true;
}
var_dump( array_filter( $array3, filter(...), ARRAY_FILTER_USE_BOTH ) );
var_dump( array_filter( $array3, filter(...), ARRAY_FILTER_USE_BOTH | ARRAY_FILTER_PACK_RESULT ) );

$array4 = [
    123,
    true,
    false,
    (object)[],
    [],
    456,
    '123',
];
var_dump( array_filter( $array4, is_int(...) ) );
var_dump( array_filter( $array4, is_int(...), ARRAY_FILTER_PACK_RESULT ) );

$array4 = [
    'first int' => 123,
    'true' => true,
    'false' => false,
    'object' => (object)[],
    'array' => [],
    'second int' => 456,
    'int-looking string' => '123',
];
var_dump( array_filter( $array4, is_int(...) ) );
var_dump( array_filter( $array4, is_int(...), ARRAY_FILTER_PACK_RESULT ) );

?>
--EXPECT--
array(3) {
  [2]=>
  bool(true)
  [4]=>
  string(3) "foo"
  [6]=>
  int(123)
}
array(3) {
  [0]=>
  bool(true)
  [1]=>
  string(3) "foo"
  [2]=>
  int(123)
}
array(1) {
  [1]=>
  bool(false)
}
array(1) {
  [0]=>
  bool(false)
}
Filter: $key => $value\nFilter: $key => $value\nFilter: $key => $value\narray(3) {
  ["key1"]=>
  bool(true)
  ["key2"]=>
  bool(false)
  ["key3"]=>
  int(123)
}
Filter: $key => $value\nFilter: $key => $value\nFilter: $key => $value\narray(3) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  int(123)
}
array(2) {
  [0]=>
  int(123)
  [5]=>
  int(456)
}
array(2) {
  [0]=>
  int(123)
  [1]=>
  int(456)
}
array(2) {
  ["first int"]=>
  int(123)
  ["second int"]=>
  int(456)
}
array(2) {
  [0]=>
  int(123)
  [1]=>
  int(456)
}
