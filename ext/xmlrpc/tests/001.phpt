--TEST--
xmlrpc_encode_request() with wrong arguments
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

var_dump(xmlrpc_encode_request(-1, 1));
var_dump(xmlrpc_encode_request("", 1));
var_dump(xmlrpc_encode_request(array(), 1));
var_dump(xmlrpc_encode_request(3.4, 1));

echo "Done\n";
?>
--EXPECTF--	
string(174) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>-1</methodName>
<params>
 <param>
  <value>
   <int>1</int>
  </value>
 </param>
</params>
</methodCall>
"
string(160) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName/>
<params>
 <param>
  <value>
   <int>1</int>
  </value>
 </param>
</params>
</methodCall>
"

Notice: Array to string conversion in %s on line %d
string(177) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>Array</methodName>
<params>
 <param>
  <value>
   <int>1</int>
  </value>
 </param>
</params>
</methodCall>
"
string(175) "<?xml version="1.0" encoding="iso-8859-1"?>
<methodCall>
<methodName>3.4</methodName>
<params>
 <param>
  <value>
   <int>1</int>
  </value>
 </param>
</params>
</methodCall>
"
Done
