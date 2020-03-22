--TEST--
xmlrpc_encode() Simple test encode array
--SKIPIF--
<?php if (!extension_loaded("xmlrpc")) print "skip"; ?>
--FILE--
<?php

$params = array(
    "one" => "red",
    "two" => "blue",
    "three" => "green"
);

$response = xmlrpc_encode($params);
echo $response;

$params = array(
    "red",
    "blue",
    "green"
);

$response = xmlrpc_encode($params);
echo $response;

$params = array(
    0 => "red",
    1 => "blue",
    3 => "green"
);

$response = xmlrpc_encode($params);
echo $response;
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <struct>
   <member>
    <name>one</name>
    <value>
     <string>red</string>
    </value>
   </member>
   <member>
    <name>two</name>
    <value>
     <string>blue</string>
    </value>
   </member>
   <member>
    <name>three</name>
    <value>
     <string>green</string>
    </value>
   </member>
  </struct>
 </value>
</param>
</params>
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <array>
   <data>
    <value>
     <string>red</string>
    </value>
    <value>
     <string>blue</string>
    </value>
    <value>
     <string>green</string>
    </value>
   </data>
  </array>
 </value>
</param>
</params>
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <struct>
   <member>
    <name>0</name>
    <value>
     <string>red</string>
    </value>
   </member>
   <member>
    <name>1</name>
    <value>
     <string>blue</string>
    </value>
   </member>
   <member>
    <name>3</name>
    <value>
     <string>green</string>
    </value>
   </member>
  </struct>
 </value>
</param>
</params>
