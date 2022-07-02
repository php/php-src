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
    unpack("Z9", "foo\0\rbar\0 \t\r\n"),
    unpack("Z2", "\0"),
    unpack("Z2", "\0\0"),
    unpack("Z2", "A\0"),
    unpack("Z2", "AB\0"),
    unpack("Z2", "ABC")
);
?>
--EXPECTF--
Warning: unpack(): Type Z: not enough input, need 2, have 1 in %s on line %d
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
bool(false)
array(1) {
  [1]=>
  string(0) ""
}
array(1) {
  [1]=>
  string(1) "A"
}
array(1) {
  [1]=>
  string(2) "AB"
}
array(1) {
  [1]=>
  string(2) "AB"
}
