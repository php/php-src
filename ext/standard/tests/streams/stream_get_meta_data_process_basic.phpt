--TEST--
Testing stream_get_meta_data() on a process stream.
--FILE--
<?php
 
$output_file = __FILE__.'.tmp'; 
$cmd = "echo here is some output";
$mode = 'rb';
$handle = popen($cmd, $mode);
$data = fread($handle, 100);

var_dump(stream_get_meta_data($handle));

pclose($handle);

echo "Done";

?>
--EXPECT--
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "rb"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
}
Done
