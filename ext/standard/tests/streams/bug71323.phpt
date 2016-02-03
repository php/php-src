--TEST--
Bug #71323: Output of stream_get_meta_data can be falsified by its input
--FILE--
<?php
$file = 'data:text/plain;z=y;uri=eviluri;mediatype=wut?;mediatype2=hello,somedata';
$meta = stream_get_meta_data(fopen($file, "r"));
var_dump($meta);
?>
--EXPECTF--
array(10) {
  ["mediatype"]=>
  string(10) "text/plain"
  ["z"]=>
  string(1) "y"
  ["uri"]=>
  string(72) "data:text/plain;z=y;uri=eviluri;mediatype=wut?;mediatype2=hello,somedata"
  ["mediatype2"]=>
  string(5) "hello"
  ["base64"]=>
  bool(false)
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
}
