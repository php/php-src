--TEST--
Bug #61525 (SOAP functions require at least one space after HTTP header colon)
--EXTENSIONS--
soap
--SKIPIF--
<?php
if (@!include __DIR__."/../../../standard/tests/http/server.inc") die('skip server.inc not available');
http_server_skipif();
?>
--FILE--
<?php
require __DIR__."/../../../standard/tests/http/server.inc";

$response = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
    <SOAP-ENV:Body>
        <ns1:AddResponse>
            <return xsi:type="xsd:int">7</return>
        </ns1:AddResponse>
    </SOAP-ENV:Body>
</SOAP-ENV:Envelope>
XML;

$length = strlen($response);
$server_response = "data://text/xml;base64," . base64_encode("HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:$length\r\n\r\n$response");
['pid' => $pid, 'uri' => $uri] = http_server([$server_response]);
$client = new SoapClient(NULL, ['location' => $uri, 'uri' => $uri]);
var_dump($client->Add(3, 4));
http_server_kill($pid);
?>
--EXPECT--
int(7)
