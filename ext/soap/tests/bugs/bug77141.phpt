--TEST--
Bug #77141 (Signedness issue in SOAP when precision=-1)
--SKIPIF--
<?php
if (!extension_loaded('soap')) die('skip soap extension not available');
?>
--FILE--
<?php
$soap = new \SoapClient(
    null,
    array(
        'location' => "http://localhost/soap.php",
        'uri' => "http://localhost/",
        'style' => SOAP_RPC,
        'trace' => true,
        'exceptions' => false,
    )
);
ini_set('precision', -1);
$soap->call(1.1);
echo $soap->__getLastRequest();
?>
===DONE===
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://localhost/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:call><param0 xsi:type="xsd:float">1.1</param0></ns1:call></SOAP-ENV:Body></SOAP-ENV:Envelope>
===DONE===
