--TEST--
Bug #49169 (SoapServer calls wrong function, although "SOAP action" header is correct)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--SKIPIF--
<?php
    if (php_sapi_name()=='cli') echo 'skip';
?>
--POST--
<SOAP-ENV:Envelope
    xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
    SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"
>
    <SOAP-ENV:Body>
        <testParam xsi:type="xsd:string">hello</testParam>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
--FILE--
<?php
function test($input) {
  return strrev($input);
}
function test2($input) {
  return strlen($input);
}

$server = new soapserver(__DIR__.'/bug49169.wsdl', []);
$server->addfunction("test");
$server->addfunction("test2");
$_SERVER["HTTP_SOAPACTION"] = "#test";
$server->handle();
$_SERVER["HTTP_SOAPACTION"] = "#test2";
$server->handle();
?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><testParam xsi:type="xsd:string">olleh</testParam></SOAP-ENV:Body></SOAP-ENV:Envelope>
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><testParam xsi:type="xsd:string">5</testParam></SOAP-ENV:Body></SOAP-ENV:Envelope>
