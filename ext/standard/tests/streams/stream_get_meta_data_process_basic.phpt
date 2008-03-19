--TEST--
Testing stream_get_meta_data() on a process stream.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    // non-windows platforms discard the 'b' from the mode string
    die('skip Not valid for Windows');
}
?>
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
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}
Done
