--TEST--
Bug #74975	Different serialization for classes
--SKIPIF--
<?php
if (!extension_loaded("xmlrpc")) print "skip";
?>
--FILE--
<?php

class Foo {

}

class Bar {

    public $xmlrpc_type;
    public $scalar;

}

$foo = new Foo();
$foo->xmlrpc_type = 'base64';
$foo->scalar = 'foobar';

$bar = new Bar();
$bar->xmlrpc_type = 'base64';
$bar->scalar = 'foobar';

echo xmlrpc_encode([
    'foo' => $foo,
    'bar' => $bar
]);

?>
--EXPECT--
<?xml version="1.0" encoding="utf-8"?>
<params>
<param>
 <value>
  <struct>
   <member>
    <name>foo</name>
    <value>
     <base64>Zm9vYmFy&#10;</base64>
    </value>
   </member>
   <member>
    <name>bar</name>
    <value>
     <base64>Zm9vYmFy&#10;</base64>
    </value>
   </member>
  </struct>
 </value>
</param>
</params>
