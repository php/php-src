--TEST--
Bug #68027 (buffer overflow in mkgmtime() function)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php

$d = '6-01-01 20:00:00';
xmlrpc_set_type($d, 'datetime');
var_dump($d);
$datetime = "2001-0-08T21:46:40-0400";
$obj = xmlrpc_decode("<?xml version=\"1.0\"?><methodResponse><params><param><value><dateTime.iso8601>$datetime</dateTime.iso8601></value></param></params></methodResponse>");
print_r($obj);

$datetime = "34770-0-08T21:46:40-0400";
$obj = xmlrpc_decode("<?xml version=\"1.0\"?><methodResponse><params><param><value><dateTime.iso8601>$datetime</dateTime.iso8601></value></param></params></methodResponse>");
print_r($obj);

echo "Done\n";
?>
--EXPECTF--
object(stdClass)#1 (3) {
  ["scalar"]=>
  string(16) "6-01-01 20:00:00"
  ["xmlrpc_type"]=>
  string(8) "datetime"
  ["timestamp"]=>
  int(%d)
}
stdClass Object
(
    [scalar] => 2001-0-08T21:46:40-0400
    [xmlrpc_type] => datetime
    [timestamp] => %s
)
stdClass Object
(
    [scalar] => 34770-0-08T21:46:40-0400
    [xmlrpc_type] => datetime
    [timestamp] => %d
)
Done
