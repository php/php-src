--TEST--
stream_get_meta_data() with a relative file path
--FILE--
<?php

echo "Create a file:\n";
$filename = __FILE__ . '.tmp';
$fp = fopen('File://' . $filename, 'w+');

var_dump(stream_get_meta_data($fp));

fclose($fp);

echo "\nChange to file's directory and open with a relative path:\n";

$dirname = dirname($filename);
chdir($dirname);
$relative_filename = basename($filename);

$fp = fopen($relative_filename, 'r');
var_dump(stream_get_meta_data($fp));

fclose($fp);

unlink($filename);

?>
--EXPECTF--
Create a file:
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "w+"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "File://%sstream_get_meta_data_file_variation4.php.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}

Change to file's directory and open with a relative path:
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["unread_chars"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "stream_get_meta_data_file_variation4.php.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
--UEXPECTF--
Create a file:
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "w+"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "File://%sstream_get_meta_data_file_variation4.php.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}

Change to file's directory and open with a relative path:
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
  unicode(%i) "stream_get_meta_data_file_variation4.php.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
