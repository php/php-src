--TEST--
pack()/unpack(): "A" modifier
--FILE--
<?php
var_dump(
	pack("A5", "foo "),
	pack("A4", "fooo"),
	pack("A4", "foo"),
	unpack("A*", "foo\0\rbar\0 \t\r\n"),
	unpack("A4", "foo\0\rbar\0 \t\r\n")
);
?>
--EXPECTF--
string(5) "foo  "
string(4) "fooo"
string(4) "foo "
array(1) {
  [1]=>
  string(8) "foo%c%cbar"
}
array(1) {
  [1]=>
  string(3) "foo"
}

