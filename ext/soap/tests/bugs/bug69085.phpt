--TEST--
Bug #69085 (SoapClient's __call() type confusion through unserialize())
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php

$dummy = unserialize('O:10:"SoapClient":5:{s:3:"uri";s:1:"a";s:8:"location";s:22:"http://localhost/a.xml";s:17:"__default_headers";i:1337;s:15:"__last_response";s:1:"a";s:5:"trace";s:1:"x";}');
try {
    $dummy->whatever();
} catch (Exception $e) {
    echo "okey";
}
?>
--EXPECT--
okey
