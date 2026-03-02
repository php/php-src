--TEST--
Bug #42654 (RecursiveIteratorIterator modifies only part of leaves)
--FILE--
<?php
$data = array ('key1' => 'val1', array('key2' => 'val2'), 'key3' => 'val3');

$iterator = new RecursiveIteratorIterator(new RecursiveArrayIterator($data));
foreach($iterator as $foo) {
    $key = $iterator->key();
    switch($key) {
        case 'key1': // first level
        case 'key2': // recursive level
            echo "update $key".PHP_EOL;
            $iterator->offsetSet($key, 'alter');
    }
}
$copy = $iterator->getArrayCopy();
var_dump($copy);
?>
--EXPECT--
update key1
update key2
array(3) {
  ["key1"]=>
  string(5) "alter"
  [0]=>
  array(1) {
    ["key2"]=>
    string(5) "alter"
  }
  ["key3"]=>
  string(4) "val3"
}
