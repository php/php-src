--TEST--
stream_get_meta_data() filters list
--FILE--
<?php

$fp = fopen(__FILE__, "r");
stream_filter_append($fp, 'string.rot13', STREAM_FILTER_READ);
stream_filter_append($fp, 'string.toupper', STREAM_FILTER_WRITE);
stream_filter_append($fp, 'string.tolower', STREAM_FILTER_ALL);

var_dump(stream_get_meta_data($fp));

fclose($fp);

?>
--EXPECTF--
array(11) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["read_filters"]=>
  array(2) {
    [0]=>
    string(12) "string.rot13"
    [1]=>
    string(14) "string.tolower"
  }
  ["write_filters"]=>
  array(2) {
    [0]=>
    string(14) "string.toupper"
    [1]=>
    string(14) "string.tolower"
  }
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%d) "%s"
}
