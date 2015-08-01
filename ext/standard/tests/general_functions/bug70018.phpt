--TEST--
Bug #70018 (exec does not strip all whitespace), var 1
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == "WIN") {
  die("skip.. not for Windows");
}
if (`echo -e test` == "-e test\n") {
  die("skip.. /bin/echo does not support -e");
}
?>
--FILE--
<?php
$output = array();
exec('/bin/echo -n -e "abc\f\n \n"',$output);
var_dump($output);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(0) ""
}
