--TEST--
Bug #71501 (xmlrpc_encode_request ignores encoding option)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php
$params = 'Lê Trung Hiếu';
echo xmlrpc_encode_request('foo', $params, ['encoding' => 'UTF-8', 'escaping' => 'markup']);
?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<methodCall>
<methodName>foo</methodName>
<params>
 <param>
  <value>
   <string>Lê Trung Hiếu</string>
  </value>
 </param>
</params>
</methodCall>
