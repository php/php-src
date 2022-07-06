--TEST--
Bug #53427 + emulate_read (stream_select does not preserve keys)
--FILE--
<?php
$read[1] = fopen(__FILE__, 'r');
$read['myindex'] = reset($read);
$write = NULL;
$except = NULL;

var_dump($read);
stream_select($read, $write, $except, 0);
var_dump($read);
fread(reset($read), 1);
stream_select($read, $write, $except, 0); // // emulate_read
var_dump($read);
?>
--EXPECTF--
array(2) {
  [1]=>
  resource(%d) of type (stream)
  ["myindex"]=>
  resource(%d) of type (stream)
}
array(2) {
  [1]=>
  resource(%d) of type (stream)
  ["myindex"]=>
  resource(%d) of type (stream)
}
array(2) {
  [1]=>
  resource(%d) of type (stream)
  ["myindex"]=>
  resource(%d) of type (stream)
}
