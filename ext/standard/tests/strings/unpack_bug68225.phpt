--TEST--
Bug #68225 unpack and X format code
--FILE--
<?php

$data = pack('VV', 1, 2);

$result = unpack('Va/X' ,$data);
var_dump($result);

$result = unpack('Va/X4' ,$data);
var_dump($result);

$result = unpack('V1a/X4/V1b/V1c/X4/V1d', $data);
var_dump($result);

?>
--EXPECT--
array(1) {
  ["a"]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(1)
}
array(4) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
  ["d"]=>
  int(2)
}
