--TEST--
Bug #47021 SoapClient (SoapClient stumbles over WSDL delivered with "Transfer-Encoding: chunked")
--INI--
soap.wsdl_cache_enabled=0
--EXTENSIONS--
soap
--SKIPIF--
<?php
require __DIR__.'/../../standard/tests/http/server.inc';
http_server_skipif();
?>
--FILE--
<?php
require __DIR__.'/../../standard/tests/http/server.inc';

function chunk_body($body, $n)
{
    $chunks = str_split($body, $n);
    $chunks[] = '';

    foreach ($chunks as $k => $v) {
        $chunks[$k] = sprintf("%08x\r\n%s\r\n", strlen($v), $v);
    }

    return join('', $chunks);
}

$wsdl = file_get_contents(__DIR__.'/server030.wsdl');

$soap = <<<EOF
<?xml version="1.0" encoding="UTF-8"?>
<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/" xmlns:ns1="http://testuri.org" xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><SOAP-ENV:Body><ns1:getItemsResponse><getItemsReturn SOAP-ENC:arrayType="ns1:Item[10]" xsi:type="ns1:ItemArray"><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text0</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text1</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text2</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text3</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text4</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text5</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text6</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text7</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text8</text></item><item xsi:type="ns1:Item"><text xsi:type="xsd:string">text9</text></item></getItemsReturn></ns1:getItemsResponse></SOAP-ENV:Body></SOAP-ENV:Envelope>
EOF;

$responses = [
    "data://text/plain,HTTP/1.1 200 OK\r\n".
    "Content-Type: text/xml;charset=utf-8\r\n".
    "Transfer-Encoding: \t  chunked\t \r\n".
    "Connection: close\r\n".
    "\r\n".
    chunk_body($wsdl, 64),
    "data://text/plain,HTTP/1.1 200 OK\r\n".
    "Content-Type: text/xml;charset=utf-8\r\n".
    "Transfer-Encoding: \t  chunked\t \r\n".
    "Connection: close\r\n".
    "\r\n".
    chunk_body($soap, 156),
];


['pid' => $pid, 'uri' => $uri] = http_server($responses);

$options = [
    'trace' => true,
    'location' => $uri,
];

class BugSoapClient extends SoapClient
{
    public function __doRequest($request, $location, $action, $version, $one_way = null): ?string
    {
        $response = parent::__doRequest($request, $location, $action, $version, $one_way);

        var_dump(strlen($response));

        return $response;
    }
}

$client = new BugSoapClient($uri, $options);

var_dump(count($client->getItems()));

http_server_kill($pid);

--EXPECT--
int(1291)
int(10)
