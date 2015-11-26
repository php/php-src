--TEST--
Bug #40576 (double values are truncated to 6 decimal digits when encoding)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=12
--FILE--
<?php

var_dump(xmlrpc_encode(1.123456789));
var_dump(xmlrpc_encode(11234567891010));
var_dump(xmlrpc_encode(11234567));
var_dump(xmlrpc_encode(""));
var_dump(xmlrpc_encode("test"));
var_dump(xmlrpc_encode("1.22222222222222222222222"));

echo "Done\n";
?>
--EXPECTF--	
string(125) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <double>1.123456789</double>
 </value>
</param>
</params>
"
string(119) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <int>-1066555326</int>
 </value>
</param>
</params>
"
string(116) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <int>11234567</int>
 </value>
</param>
</params>
"
string(106) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <string/>
 </value>
</param>
</params>
"
string(118) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <string>test</string>
 </value>
</param>
</params>
"
string(139) "<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <string>1.22222222222222222222222</string>
 </value>
</param>
</params>
"
Done
