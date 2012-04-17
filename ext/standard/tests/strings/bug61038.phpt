--TEST--
BugFix #61038
--FILE--
<?php
	var_dump(unpack("Z4", pack("Z4", "foo")));
	var_dump(unpack("a4", pack("a4", "foo")));
	var_dump(unpack("A4", pack("A4", "foo")));
	var_dump(unpack("a9", pack("a*", "foo\x00bar\x00 ")));
	var_dump(unpack("A9", pack("a*", "foo\x00bar\x00 ")));
	var_dump(unpack("Z9", pack("a*", "foo\x00bar\x00 ")));
?>
--EXPECTF--
array(1) {
  [1]=>
  string(3) "foo"
}
array(1) {
  [1]=>
  string(4) "foo%c"
}
array(1) {
  [1]=>
  string(3) "foo"
}
array(1) {
  [1]=>
  string(9) "foo%cbar%c "
}
array(1) {
  [1]=>
  string(7) "foo%cbar"
}
array(1) {
  [1]=>
  string(3) "foo"
}

