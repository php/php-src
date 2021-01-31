--TEST--
ImmutableIterable constructed from array
--FILE--
<?php

$it = new ImmutableIterable(['first' => 'x', 'second' => new stdClass()]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
// TODO: Change var_dump representation to [['key' => $key, 'value' => $value],...]
var_dump($it);
var_dump((array)$it);

$it = new ImmutableIterable([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

?>
--EXPECT--
Key: 'first'
Value: 'x'
Key: 'second'
Value: (object) array(
)
object(ImmutableIterable)#1 (2) {
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
    string(6) "second"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
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
    string(6) "second"
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
object(ImmutableIterable)#3 (0) {
}
array(0) {
}