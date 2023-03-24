--TEST--
oss-fuzz #57392: str_getcsv() with null byte as delimiter and enclosure
--FILE--
<?php
var_dump(str_getcsv(
    "aaaaaaaaaaaa\0  ",
    "\0",
    "\0",
));
?>
--EXPECT--
array(2) {
  [0]=>
  string(12) "aaaaaaaaaaaa"
  [1]=>
  string(2) "  "
}
