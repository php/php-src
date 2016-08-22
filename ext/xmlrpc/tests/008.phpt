--TEST--
xmlrpc_decode() Simple test decode type i8

--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) die("skip");
?>

--FILE--
<?php

$xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <i8>1</i8>
 </value>
</param>
</params>
XML;

$response = xmlrpc_decode($xml);
echo $response;

--EXPECT--
1
