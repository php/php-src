--TEST--
GH-18990 (SOAP HTTP socket not closing on object destruction)
--INI--
soap.wsdl_cache_enabled=0
--EXTENSIONS--
soap
--SKIPIF--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';
http_server_skipif();
--FILE--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';

$wsdl = file_get_contents(__DIR__.'/../server030.wsdl');

$soap = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getItemsResponse><getItemsReturn SOAP-ENC:arrayType="ns1:Item[10]" xsi:type="ns1:ItemArray"><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text0</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text1</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text2</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text3</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text4</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text5</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text6</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text7</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text8</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text9</text></item></getItemsReturn></ns1:getItemsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

$responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\n".
    "Content-Type: text/xml;charset=utf-8\r\n".
    "Connection: Keep-Alive\r\n".
    "Content-Length: ".strlen($wsdl)."\r\n".
    "\r\n".
    $wsdl,

    "data://text/plain,HTTP/1.1 200 OK\r\n".
    "Content-Type: text/xml;charset=utf-8\r\n".
    "Connection: Keep-Alive\r\n".
    "Content-Length: ".strlen($soap)."\r\n".
    "\r\n".
    $soap,
];

['pid' => $pid, 'uri' => $uri] = http_server($responses);

$options = [
    'trace' => false,
    'location' => $uri,
];

$cnt = count(get_resources());

$client = new SoapClient($uri, $options);

var_dump(count($client->getItems()));

http_server_kill($pid);

unset($client);
var_dump(count(get_resources()) - $cnt);
?>
--EXPECT--
int(10)
int(0)
