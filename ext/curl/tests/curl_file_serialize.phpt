--TEST--
CURL file uploading
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
?>
--FILE--
<?php
$data = 'a:2:{s:4:"file";O:8:"CURLFile":4:{s:4:"name";s:0:"";s:6:"buffer";s:9:"testdata2";s:4:"mime";s:9:"mime_type";s:8:"postname";s:9:"post_name";}s:4:"data";s:3:"foo";}';
var_dump(unserialize($data));

$data = 'a:2:{s:4:"file";O:8:"CURLFile":4:{s:4:"name";s:6:"buffer";s:0:"";s:13:"testdata1.txt";s:4:"mime";s:0:"";s:8:"postname";s:0:"";}s:4:"data";s:3:"foo";}';
var_dump(unserialize($data));
?>
--EXPECTF--
array(2) {
  ["file"]=>
  object(CURLFile)#1 (4) {
    ["name"]=>
    string(0) ""
    ["buffer"]=>
    string(9) "testdata2"
    ["mime"]=>
    string(9) "mime_type"
    ["postname"]=>
    string(9) "post_name"
  }
  ["data"]=>
  string(3) "foo"
}

Fatal error: Uncaught Exception: Unserialization of CURLFile instances with file name is not allowed in %s
Stack trace:
#0 [internal function]: CURLFile->__wakeup()
#1 %s
#2 {main}
  thrown in %s on line %d

