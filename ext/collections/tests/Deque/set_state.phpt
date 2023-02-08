--TEST--
Collections\Deque::__set_state
--FILE--
<?php

function dump_repr($obj) {
    echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
}
dump_repr(Collections\Deque::__set_state([]));
Collections\Deque::__set_state(['first' => 'x']);
$it = Collections\Deque::__set_state([strtoupper('a literal'), ['first', 'x'], [(object)['key' => 'value'], null]]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
dump_repr($it);
var_dump($it);
var_dump((array)$it);

?>
--EXPECT--
\Collections\Deque::__set_state(array(
))
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
\Collections\Deque::__set_state(array(
   0 => 'A LITERAL',
   1 =>
  array (
    0 => 'first',
    1 => 'x',
  ),
   2 =>
  array (
    0 =>
    (object) array(
       'key' => 'value',
    ),
    1 => NULL,
  ),
))
object(Collections\Deque)#2 (3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
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
array(3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
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