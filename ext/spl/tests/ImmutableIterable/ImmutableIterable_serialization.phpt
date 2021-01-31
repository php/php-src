--TEST--
ImmutableIterable can be serialized and unserialized
--FILE--
<?php

$it = new ImmutableIterable(['first' => new stdClass()]);
try {
    $it->dynamicProp = 123;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
$ser = serialize($it);
echo $ser, "\n";
foreach (unserialize($ser) as $key => $value) {
    echo "Entry:\n";
    var_dump($key, $value);
}
var_dump($ser === serialize($it));
echo "Done\n";
$x = 123;
$it = ImmutableIterable::fromPairs([[new stdClass(), &$x]]);
var_dump($it->__serialize());
$x = 'modified'; // no effect, this is copied by value
var_dump($it->__serialize());
$it = new ImmutableIterable([]);
var_dump($it->__serialize());

$it = ImmutableIterable::fromPairs([[new stdClass(), &$x]]);
echo "json encoded: " . var_export(json_encode($it), true) . "\n";
echo "json encoded empty: " . var_export(json_encode($it), true) . "\n";
var_dump($it);
?>
--EXPECT--
Caught Error: Cannot create dynamic property ImmutableIterable::$dynamicProp
O:17:"ImmutableIterable":2:{i:0;s:5:"first";i:1;O:8:"stdClass":0:{}}
Entry:
string(5) "first"
object(stdClass)#5 (0) {
}
bool(true)
Done
array(2) {
  [0]=>
  object(stdClass)#6 (0) {
  }
  [1]=>
  int(123)
}
array(2) {
  [0]=>
  object(stdClass)#6 (0) {
  }
  [1]=>
  int(123)
}
array(0) {
}
json encoded: '[[{},"modified"]]'
json encoded empty: '[[{},"modified"]]'
object(ImmutableIterable)#6 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#4 (0) {
    }
    [1]=>
    string(8) "modified"
  }
}