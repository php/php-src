--TEST--
xmlrpc_encode() Simple test encode int
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$response = xmlrpc_encode(1);
echo $response;

--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <int>1</int>
 </value>
</param>
</params>