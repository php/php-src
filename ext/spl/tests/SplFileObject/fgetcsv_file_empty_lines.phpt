--TEST--
SplFileObject::fgetcsv with empty lines
--FILE--
<?php

$file = new SplTempFileObject();

$file->fwrite("foo,bar\n");
$file->fwrite("\n");
$file->fwrite("baz,qux");

$file->rewind();


var_dump($file->fgetcsv());
var_dump($file->fgetcsv());
var_dump($file->fgetcsv());
var_dump($file->fgetcsv());

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
array(1) {
  [0]=>
  NULL
}
array(2) {
  [0]=>
  string(3) "baz"
  [1]=>
  string(3) "qux"
}
bool(false)
