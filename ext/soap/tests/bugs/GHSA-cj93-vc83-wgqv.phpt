--TEST--
GHSA-cj93-vc83-wgqv
--INI--
soap.wsdl_cache_enabled=0
memory_limit=-1
--EXTENSIONS--
soap
--CONFLICTS--
all
--SKIPIF--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';
http_server_skipif();
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
if (!getenv("SOAP_RUN_RESOURCE_HEAVY_TESTS")) die("skip SOAP resource-heavy test");
if (PHP_INT_SIZE == 4) die('skip not for 32-bit systems due to memory constraints');

if (!file_exists('/proc/meminfo')) {
    die('skip Cannot check free RAM from /proc/meminfo on this platform');
}

$free_ram = 0;
if ($f = fopen("/proc/meminfo","r")) {
    while (!feof($f)) {
        if (preg_match('/MemFree[^\d]*(\d+)/i', fgets($f), $m)) {
            $free_ram = max($free_ram, $m[1]/1024/1024);
            if ($free_ram > 4) {
                $enough_free_ram = true;
            }
        }
    }
}

if (empty($enough_free_ram)) {
    die(sprintf("skip need +4G free RAM, but only %01.2f available", $free_ram));
}
--FILE--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';

function chunk_body($body, $n)
{
    $chunks = str_split($body, $n);
    $chunks[] = '';

    foreach ($chunks as $k => $v) {
        $chunks[$k] = sprintf("%08x\r\n%s\r\n", strlen($v), $v);
    }

    return join('', $chunks);
}

$wsdl = file_get_contents(__DIR__.'/../server030.wsdl');

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
    /* The second chunk only needs its size header: the reallocation for it
     * happens before its body is read, so the overflow triggers on the first
     * read into the undersized buffer. */
    sprintf("%08x\r\n", 0x7fffffff).str_repeat('x', 0x7fffffff)."\r\n" .
    sprintf("%08x\r\n", 0x7fffffff)."xxxx",
];


['pid' => $pid, 'uri' => $uri] = http_server($responses);

$options = [
    'trace' => false,
    'location' => $uri,
];

$client = new SoapClient($uri, $options);

$client->getItems();

http_server_kill($pid);

--EXPECTF--
Fatal error: Uncaught SoapFault exception: [HTTP] Error Fetching http body, No Content-Length, connection closed or chunked data in %s:%d
Stack trace:
#0 [internal function]: SoapClient->__doRequest('<?xml version="...', '%s', 'http://testuri....', 1, false)
#1 %s(%d): SoapClient->__call('getItems', Array)
#2 {main}
  thrown in %s on line %d
