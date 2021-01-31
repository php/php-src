--TEST--
ImmutableIterable::fromPairs
--FILE--
<?php

try {
    ImmutableIterable::fromPairs(['first' => 'x']);
} catch (Throwable $e) {
    printf("Caught %s: %s\n", $e::class, $e->getMessage());
}
$it = ImmutableIterable::fromPairs([['first', 'x'], [(object)['key' => 'value'], null]]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
$pairsIterable = call_user_func(function () { yield new stdClass() => [strtolower('KEY'), strtolower('OTHER')]; });
$it = ImmutableIterable::fromPairs($pairsIterable);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
var_dump($it->toPairs());

?>
--EXPECT--
Caught UnexpectedValueException: Expected to find pair in array but got non-array
key="first" value="x"
key={"key":"value"} value=null
key="key" value="other"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "key"
    [1]=>
    string(5) "other"
  }
}