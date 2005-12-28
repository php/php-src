--TEST--
Bug #35817 (unpack() does not decode odd number of hexadecimal values)
--FILE--
<?php
$a = pack("H3","181");
$b = unpack("H3", $a);
var_dump($b);

$a = pack("H2","18");
$b = unpack("H2", $a);
var_dump($b);

$a = pack("H","1");
$b = unpack("H", $a);
var_dump($b);
?>
--EXPECT--
array(1) {
  [1]=>
  string(3) "181"
}
array(1) {
  [1]=>
  string(2) "18"
}
array(1) {
  [1]=>
  string(1) "1"
}
