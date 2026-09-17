--TEST--
GHSA-rgrp-mwpx-f6rm: Stack overflow on a chain of href references
--EXTENSIONS--
soap
--FILE--
<?php

/* The body stays four deep whatever $n is: the decoder walks the href chain. */
function chain(int $n): string
{
    $links = '';
    for ($i = 1; $i <= $n; $i++) {
        $links .= '<x id="i' . $i . '">' . ($i < $n ? '<q href="#i' . ($i + 1) . '"/>' : 'leaf') . '</x>';
    }

    return '<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body><test><p href="#i1"/>'
        . $links . '</test></s:Body></s:Envelope>';
}

function test($arg)
{
    $GLOBALS['decoded'] = $arg;

    return 'ok';
}

$server = new SoapServer(null, ['uri' => 'urn:test']);
$server->addFunction('test');

$server->handle(chain(3));
var_dump($GLOBALS['decoded']);

$server->handle(chain(12000));

?>
--EXPECTF--
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="urn:test" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:testResponse><return xsi:type="xsd:string">ok</return></ns1:testResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
object(stdClass)#%d (1) {
  ["q"]=>
  object(stdClass)#%d (1) {
    ["q"]=>
    string(4) "leaf"
  }
}
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"><SOAP-ENV:Body><SOAP-ENV:Fault><faultcode>SOAP-ENV:Server</faultcode><faultstring>SOAP-ERROR: Encoding: Nesting level too deep</faultstring></SOAP-ENV:Fault></SOAP-ENV:Body></SOAP-ENV:Envelope>
