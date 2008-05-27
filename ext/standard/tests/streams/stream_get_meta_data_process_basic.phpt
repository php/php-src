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
array(8) {
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "rb"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(false)
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
Done
