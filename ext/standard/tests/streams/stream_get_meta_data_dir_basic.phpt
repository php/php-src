--TEST--
stream_get_meta_data() on directories
--FILE--
<?php

$dir = opendir(dirname(__FILE__));
var_dump(stream_get_meta_data($dir));
closedir($dir);

$dirObject = dir(dirname(__FILE__));
var_dump(stream_get_meta_data($dirObject->handle));

?>
--EXPECT--
array(8) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(3) "dir"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
}
array(8) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(3) "dir"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
}
