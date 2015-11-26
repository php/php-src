--TEST--
testing stream_get_meta_data() "eof" field for a file stream
--FILE--
<?php

$filename = __FILE__ . '.tmp';

$fp = fopen($filename, "w+");

echo "Write some data to the file:\n";
$i = 0;
while ($i++ < 20) {
	fwrite($fp, "a line of data\n");
}

var_dump(stream_get_meta_data($fp));

//seek to start of file
rewind($fp);

echo "\n\nRead entire file:\n";
while(!feof($fp)) {
	fread($fp, 1);
}

var_dump(stream_get_meta_data($fp));

fclose($fp);

unlink($filename);

?>
--EXPECTF--
Write some data to the file:
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
  string(%i) "%s"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}


Read entire file:
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
  string(%i) "%s"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(true)
}
