--TEST--
Bug #72647 (xmlrpc_encode() unexpected output after referencing array elements)
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php

$ar = array(4, "a", 7);
$v = &$ar[1];
unset($v);

echo xmlrpc_encode($ar);
?>
--EXPECTF--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <array>
   <data>
    <value>
     <int>4</int>
    </value>
    <value>
     <string>a</string>
    </value>
    <value>
     <int>7</int>
    </value>
   </data>
  </array>
 </value>
</param>
</params>
