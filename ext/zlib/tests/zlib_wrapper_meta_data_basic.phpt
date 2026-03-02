--TEST--
Test function stream_get_meta_data on a zlib stream
--EXTENSIONS--
zlib
--FILE--
<?php
echo "no wrapper\n";
$f = __DIR__."/data/test.txt.gz";
$h = gzopen($f,'r');
var_dump(stream_get_meta_data($h));
gzclose($h);
echo "\nwith wrapper\n";
$f2 = "compress.zlib://" . $f;
$h = fopen($f2,'r');
var_dump(stream_get_meta_data($h));
gzclose($h);


?>
--EXPECTF--
no wrapper
array(7) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["stream_type"]=>
  string(4) "ZLIB"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
}

with wrapper
array(9) {
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
  ["wrapper_type"]=>
  string(4) "ZLIB"
  ["stream_type"]=>
  string(4) "ZLIB"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%d) "compress.zlib://%s/test.txt.gz"
}
