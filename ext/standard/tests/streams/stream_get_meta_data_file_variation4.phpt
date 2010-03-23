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
array(9) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "w+"
  ["unread_bytes"]=>
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
array(9) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
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
