--TEST--
Test Csv\LazyLaxCollection::getIterator()
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour,Five,Six\r\nSeven,Eight,Nine\r\n";

$csv = Csv\LazyLaxCollection::createFromBuffer($string);
$it = $csv->getIterator();
var_dump($it);

while ($it->valid()) {
    var_dump($it->key());
    var_dump($it->current());
    $it->next();
}

?>
--EXPECT--
object(InternalIterator)#3 (0) {
}
int(0)
array(3) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
}
int(1)
array(3) {
  [0]=>
  string(4) "Four"
  [1]=>
  string(4) "Five"
  [2]=>
  string(3) "Six"
}
int(2)
array(3) {
  [0]=>
  string(5) "Seven"
  [1]=>
  string(5) "Eight"
  [2]=>
  string(4) "Nine"
}
