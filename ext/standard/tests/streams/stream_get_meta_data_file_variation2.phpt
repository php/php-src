--TEST--
Testing stream_get_meta_data() "unread_bytes" field
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

echo "\n\nRead a line of the file, causing data to be buffered:\n";
var_dump(fgets($fp));

var_dump(stream_get_meta_data($fp));

echo "\n\nRead 20 bytes from the file:\n";
fread($fp, 20);

var_dump(stream_get_meta_data($fp));

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
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}


Read a line of the file, causing data to be buffered:
string(15) "a line of data
"
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "w+"
  ["unread_bytes"]=>
  int(285)
  ["unread_chars"]=>
  int(285)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}


Read 20 bytes from the file:
array(10) {
  ["wrapper_type"]=>
  string(9) "plainfile"
  ["stream_type"]=>
  string(5) "STDIO"
  ["mode"]=>
  string(2) "w+"
  ["unread_bytes"]=>
  int(265)
  ["unread_chars"]=>
  int(265)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(false)
}


Read entire file:
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
  string(%i) "%s.tmp"
  ["timed_out"]=>
  bool(false)
  ["blocked"]=>
  bool(true)
  ["eof"]=>
  bool(true)
}
--UEXPECTF--
Write some data to the file:

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d

Notice: fwrite(): 15 character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
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
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Read a line of the file, causing data to be buffered:
string(15) "a line of data
"
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "w+"
  [u"unread_bytes"]=>
  int(285)
  [u"unread_chars"]=>
  int(285)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Read 20 bytes from the file:
array(10) {
  [u"wrapper_type"]=>
  unicode(9) "plainfile"
  [u"stream_type"]=>
  unicode(5) "STDIO"
  [u"mode"]=>
  unicode(2) "w+"
  [u"unread_bytes"]=>
  int(265)
  [u"unread_chars"]=>
  int(265)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(false)
}


Read entire file:
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
  unicode(%i) "%s.tmp"
  [u"timed_out"]=>
  bool(false)
  [u"blocked"]=>
  bool(true)
  [u"eof"]=>
  bool(true)
}
