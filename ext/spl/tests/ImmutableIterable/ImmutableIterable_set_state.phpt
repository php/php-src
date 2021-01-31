--TEST--
ImmutableIterable::__set_state
--FILE--
<?php

function dump_repr($obj) {
    echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
}
dump_repr(ImmutableIterable::__set_state([]));
try {
    ImmutableIterable::__set_state(['first' => 'x']);
} catch (Throwable $e) {
    printf("Caught %s: %s\n", $e::class, $e->getMessage());
}
$it = ImmutableIterable::__set_state([['first', 'x'], [(object)['key' => 'value'], null]]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
dump_repr($it);
var_dump($it->toPairs());

?>
--EXPECT--
ImmutableIterable::__set_state(array(
))
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key="first" value="x"
key={"key":"value"} value=null
ImmutableIterable::__set_state(array(
   0 =>
  array (
    0 => 'first',
    1 => 'x',
  ),
   1 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [1]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}