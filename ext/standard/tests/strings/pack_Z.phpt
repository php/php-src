--TEST--
pack()/unpack(): "Z" format
--FILE--
<?php
var_dump(
	pack("Z0", "f"),
    pack("Z5", "foo\0"),
    pack("Z4", "fooo"),
    pack("Z4", "foo"),
	pack("Z*", "foo"),
    unpack("Z*", "foo\0\rbar\0 \t\r\n"),
    unpack("Z9", "foo\0\rbar\0 \t\r\n")
);
--EXPECTF--
string(0) ""
string(5) "foo%c%c"
string(4) "foo%c"
string(4) "foo%c"
string(4) "foo%c"
array(1) {
  [1]=>
  string(3) "foo"
}
array(1) {
  [1]=>
  string(3) "foo"
}
