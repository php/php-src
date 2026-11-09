--TEST--
NUL bytes in error messages are not truncated with xmlrpc_errors
--INI--
xmlrpc_errors=1
--FILE--
<?php
new class { abstract public $p { get; } };
?>
--EXPECTF--
<?xml version="1.0"?><methodResponse><fault><value><struct><member><name>faultCode</name><value><int>0</int></value></member><member><name>faultString</name><value><string>Fatal error:Class class@anonymous%0%s must implement 1 abstract method (class@anonymous%0%s::$p::get) in %s on line %d</string></value></member></struct></value></fault></methodResponse>
