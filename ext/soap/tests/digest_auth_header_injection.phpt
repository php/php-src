--TEST--
SoapClient must truncate Digest authentication values that contain newline characters
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

['pid' => $pid, 'uri' => $uri] = http_server([
    "data://text/plain,HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Digest realm=\"r\rX-Realm: yes\", nonce=\"n\", qop=\"auth\"\r\nContent-Length: 0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Type: text/xml\r\nContent-Length: " . strlen($response) . "\r\n\r\n" . $response,
], $capture);

$client = new SoapClient(null, [
    'location' => "$uri/",
    'uri' => 'test-uri',
    'login' => "user\r\nX-Injected: yes",
    'password' => 'secret',
    'authentication' => SOAP_AUTHENTICATION_DIGEST,
    'keep_alive' => false,
]);
var_dump($client->__soapCall('foo', []));

http_server_kill($pid);

rewind($capture);
$requests = stream_get_contents($capture);
var_dump(preg_match_all('/^Authorization: Digest username="user", realm="r", nonce="n"/m', $requests));
var_dump(preg_match('/^X-Injected:/m', $requests));
var_dump(str_contains($requests, "\rX-Realm"));
?>
--EXPECTF--
Warning: SoapClient::__doRequest(): Header Authorization value contains newline characters and has been truncated in %s on line %d

Warning: SoapClient::__doRequest(): Header Authorization value contains newline characters and has been truncated in %s on line %d
string(2) "ok"
int(1)
int(0)
bool(false)
