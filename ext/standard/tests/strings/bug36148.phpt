--TEST--
Bug #36148 (unpack("H*hex", $data) is adding an extra character to the end of the string)
--FILE--
<?php
$values = array("a", "aa", "aaa", "aaaa");
foreach ($values as $value) {
	$a = pack("H*", $value);
	$b = unpack("H*", $a);
	echo $value.": ";
	var_dump($b);
}
?>
--EXPECT--
a: array(1) {
  [1]=>
  string(2) "a0"
}
aa: array(1) {
  [1]=>
  string(2) "aa"
}
aaa: array(1) {
  [1]=>
  string(4) "aaa0"
}
aaaa: array(1) {
  [1]=>
  string(4) "aaaa"
}
