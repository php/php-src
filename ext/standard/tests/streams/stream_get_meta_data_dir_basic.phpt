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
array(9) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(3) "dir"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
array(9) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(3) "dir"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
--UEXPECT--
array(9) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(3) "dir"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
array(9) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(3) "dir"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
