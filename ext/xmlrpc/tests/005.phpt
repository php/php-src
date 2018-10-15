--TEST--
xmlrpc_encode() Simple test encode type double and String
--CREDITS--
Michel Araujo <araujo_michel@yahoo.com.br>
#PHPSP 2013-08-22
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$response = xmlrpc_encode(3.24234);
echo $response;

$response = xmlrpc_encode(-3.24234);
echo $response;

$response = xmlrpc_encode('Is string');
echo $response;
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <double>3.24234</double>
 </value>
</param>
</params>
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <double>-3.24234</double>
 </value>
</param>
</params>
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <string>Is string</string>
 </value>
</param>
</params>
