--TEST--
SoapClient::__doRequest() doesn't reach hard limit through a connection-close body
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

/* No Content-Length and no Transfer-Encoding, so get_http_body() takes the
 * header_close branch and grows the buffer 4096 bytes at a time until EOF. */
$server = oom_soap_http_server(
    "HTTP/1.1 200 OK\r\nContent-Type: text/xml\r\nConnection: close\r\n\r\n",
    str_repeat('x', 32),
    1024 * 1024
);

$client = new SoapClient(null, ['location' => $server['uri'], 'uri' => 'http://test-uri/']);
$client->__doRequest('<?xml version="1.0"?><E/>', $server['uri'], 'f', SOAP_1_1);
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
