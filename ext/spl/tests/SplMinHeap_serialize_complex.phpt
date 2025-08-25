--TEST--
SplMinHeap serialization with nested arrays
--FILE--
<?php
$heap = new SplMinHeap();
$heap->insert(['name' => 'Alice', 'nested' => ['age' => 25, 'city' => 'NYC']]);
$heap->insert(['name' => 'Bob', 'nested' => ['age' => 30, 'city' => 'LA']]);
$heap->insert(['name' => 'Charlie', 'nested' => ['age' => 35, 'city' => 'SF']]);

$serialized = serialize($heap);
echo $serialized . "\n";

$unserialized = unserialize($serialized);
var_dump($unserialized);

?>
--EXPECT--
O:10:"SplMinHeap":2:{i:0;a:0:{}i:1;a:2:{s:5:"flags";i:0;s:13:"heap_elements";a:3:{i:0;a:2:{s:4:"name";s:5:"Alice";s:6:"nested";a:2:{s:3:"age";i:25;s:4:"city";s:3:"NYC";}}i:1;a:2:{s:4:"name";s:3:"Bob";s:6:"nested";a:2:{s:3:"age";i:30;s:4:"city";s:2:"LA";}}i:2;a:2:{s:4:"name";s:7:"Charlie";s:6:"nested";a:2:{s:3:"age";i:35;s:4:"city";s:2:"SF";}}}}}
object(SplMinHeap)#2 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(3) {
    [0]=>
    array(2) {
      ["name"]=>
      string(5) "Alice"
      ["nested"]=>
      array(2) {
        ["age"]=>
        int(25)
        ["city"]=>
        string(3) "NYC"
      }
    }
    [1]=>
    array(2) {
      ["name"]=>
      string(3) "Bob"
      ["nested"]=>
      array(2) {
        ["age"]=>
        int(30)
        ["city"]=>
        string(2) "LA"
      }
    }
    [2]=>
    array(2) {
      ["name"]=>
      string(7) "Charlie"
      ["nested"]=>
      array(2) {
        ["age"]=>
        int(35)
        ["city"]=>
        string(2) "SF"
      }
    }
  }
}
