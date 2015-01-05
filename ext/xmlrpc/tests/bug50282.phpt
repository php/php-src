--TEST--
Bug #50282 (xmlrpc_encode_request() changes object into array in calling function)
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

class One { var $x = 10; }

$o = new One();
var_dump($o);
var_dump(xmlrpc_encode_request('test', $o));
var_dump($o);

?>
--EXPECTF--
object(One)#%d (1) {
  ["x"]=>
  int(10)
}
string(279) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>test</methodName>
<params>
 <param>
  <value>
   <struct>
    <member>
     <name>x</name>
     <value>
      <int>10</int>
     </value>
    </member>
   </struct>
  </value>
 </param>
</params>
</methodCall>
"
object(One)#%d (1) {
  ["x"]=>
  int(10)
}
