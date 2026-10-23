--TEST--
SoapClient ignores cookies set by a server for a domain that does not match its host
--EXTENSIONS--
soap
--SKIPIF--
<?php
require __DIR__ . '/../../standard/tests/http/server.inc';
http_server_skipif();
?>
--FILE--
<?php
require __DIR__ . '/../../standard/tests/http/server.inc';

$response = '<?xml version="1.0" encoding="UTF-8"?>'
    . '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">'
    . '<SOAP-ENV:Body><fooResponse><result>ok</result></fooResponse></SOAP-ENV:Body>'
    . '</SOAP-ENV:Envelope>';

['pid' => $pid, 'uri' => $uri] = http_server(function ($server) use ($response) {
    $host = parse_url('http://' . stream_socket_get_name($server, false), PHP_URL_HOST);
    yield "data://text/plain,HTTP/1.1 200 OK\r\n"
        . "Set-Cookie: foreign=1; domain=evil.example\r\n"
        . "Set-Cookie: suffix=2; domain=.ple\r\n"
        . "Set-Cookie: own=3; domain=$host\r\n"
        . "Set-Cookie: hostonly=4; path=/\r\n"
        . "Content-Type: text/xml\r\nContent-Length: " . strlen($response) . "\r\n\r\n" . $response;
});

$client = new SoapClient(null, [
    'location' => "$uri/",
    'uri' => 'test-uri',
    'keep_alive' => false,
]);
var_dump($client->__soapCall('foo', []));

http_server_kill($pid);

$cookies = array_keys($client->__getCookies());
sort($cookies);
var_dump($cookies);
?>
--EXPECT--
string(2) "ok"
array(2) {
  [0]=>
  string(8) "hostonly"
  [1]=>
  string(3) "own"
}
