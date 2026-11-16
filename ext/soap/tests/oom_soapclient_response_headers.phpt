--TEST--
SoapClient::__doRequest() doesn't reach hard limit through response headers
--EXTENSIONS--
soap
--SKIPIF--
<?php
require __DIR__ . '/oom_http_server.inc';
oom_soap_http_server_skipif();
?>
--INI--
memory_limit=8M
--FILE--
<?php
require __DIR__ . '/oom_http_server.inc';

$server = oom_soap_http_server(
    "HTTP/1.1 200 OK\r\n",
    "X-Pad: " . str_repeat('a', 4000) . "\r\n",
    8 * 1024
);

$client = new SoapClient(null, ['location' => $server['uri'], 'uri' => 'http://test-uri/']);
$client->__doRequest('<?xml version="1.0"?><E/>', $server['uri'], 'f', SOAP_1_1);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
