--TEST--
stream_get_meta_data() on zip stream
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = __DIR__ . '/';
$file = $dirname . 'test_with_comment.zip';
include $dirname . 'utils.inc';
$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
$fp = $zip->getStream('foo');

if(!$fp) exit("\n");

var_dump(stream_get_meta_data($fp));

fclose($fp);
$zip->close();


$fp = fopen('zip://' . __DIR__ . '/test_with_comment.zip#foo', 'rb');
if (!$fp) {
  exit("cannot open\n");
}

var_dump(stream_get_meta_data($fp));
fclose($fp);

?>
--EXPECTF--
array(8) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(3) "zip"
  ["mode"]=>
  string(2) "rb"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["uri"]=>
  string(3) "foo"
}
array(9) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["wrapper_type"]=>
  string(11) "zip wrapper"
  ["stream_type"]=>
  string(3) "zip"
  ["mode"]=>
  string(2) "rb"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(false)
  ["uri"]=>
  string(%d) "zip://%stest_with_comment.zip#foo"
}
