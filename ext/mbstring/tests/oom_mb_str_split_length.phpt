--TEST--
mb_str_split() doesn't over allocate - split length argument
--EXTENSIONS--
mbstring
--INI--
memory_limit=8M
--FILE--
<?php
var_dump(mb_str_split("abc", 100 * 1024 * 1024, "UTF-7"));
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "abc"
}
