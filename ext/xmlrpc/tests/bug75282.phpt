--TEST--
Bug #75282 (xmlrpc_encode_request() crashes)
--SKIPIF--
<?php
if (!extension_loaded('xmlrpc')) die('skip xmlrpc extension not available');
?>
--FILE--
<?php
echo xmlrpc_encode_request('func', 'text', []);
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>func</methodName>
<params>
 <param>
  <value>
   <string>text</string>
  </value>
 </param>
</params>
</methodCall>
===DONE===
