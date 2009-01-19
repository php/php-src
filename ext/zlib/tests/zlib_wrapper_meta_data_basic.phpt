--TEST--
Test function stream_get_meta_data on a zlib stream
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded"; 
}
?>
--FILE--
<?php
echo "no wrapper\n";
$f = dirname(__FILE__)."/004.txt.gz";
$h = gzopen($f,'r');	
var_dump(stream_get_meta_data($h));
gzclose($h);
echo "\nwith wrapper\n";
$f = "compress.zlib://".dirname(__FILE__)."/004.txt.gz";
$h = fopen($f,'r');	
var_dump(stream_get_meta_data($h));
gzclose($h);


?>
===DONE===
--EXPECTF--
no wrapper
array(8) {
  [u"stream_type"]=>
  unicode(4) "ZLIB"
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

with wrapper
array(10) {
  [u"wrapper_type"]=>
  unicode(4) "ZLIB"
  [u"stream_type"]=>
  unicode(4) "ZLIB"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%d) "compress.zlib://%s/004.txt.gz"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}
===DONE===