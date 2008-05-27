--TEST--
stream_get_meta_data() basic functionality
--FILE--
<?php

$fp = fopen(__FILE__, "r");

var_dump(stream_get_meta_data($fp));

fclose($fp);

?>
--EXPECTF--
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%sstream_get_meta_data_file_basic.php"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
