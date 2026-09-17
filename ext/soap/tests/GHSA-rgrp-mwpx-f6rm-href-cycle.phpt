--TEST--
GHSA-rgrp-mwpx-f6rm: Stack overflow on an array element referencing its own array
--EXTENSIONS--
soap
--FILE--
<?php

function envelope(string $param): string
{
    return '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"'
        . ' xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
        . ' xmlns:e="http://schemas.xmlsoap.org/soap/encoding/"'
        . ' s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">'
        . '<s:Body><test>' . $param . '</test></s:Body></s:Envelope>';
}

function test($arg)
{
    $GLOBALS['decoded'] = $arg;

    return 'ok';
}

$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('test');

$server->handle(envelope('<param xsi:type="e:Array" e:arrayType="xsi:string[2]"><i>a</i><i>b</i></param>'));
var_dump($GLOBALS['decoded']);

$server->handle(envelope('<param xsi:type="e:Array" e:arrayType="xsi:anyType[1]" id="x"><i href="#x"/></param>'));

?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">ok</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SOAP-ERROR: Encoding: Nesting level too deep</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
