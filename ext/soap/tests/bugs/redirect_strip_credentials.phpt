--TEST--
SOAP client does not send credentials to another origin after a redirect
--EXTENSIONS--
soap
--SKIPIF--
<?php
require __DIR__ . '/../../../standard/tests/http/server.inc';
http_server_skipif();
?>
--FILE--
<?php
require __DIR__ . '/../../../standard/tests/http/server.inc';

function report(string $label, string $requests): void {
    echo $label, "\n";
    foreach (['Authorization', 'Cookie', 'X-Custom'] as $name) {
        echo "  $name: ", preg_match_all('/^' . $name . ':/mi', $requests), "\n";
    }
}

$response = '<?xml version="1.0" encoding="UTF-8"?>'
    . '<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/">'
    . '<SOAP-ENV:Body><fooResponse><result>ok</result></fooResponse></SOAP-ENV:Body>'
    . '</SOAP-ENV:Envelope>';
$ok = "data://text/plain,HTTP/1.1 200 OK\r\nContent-Type: text/xml\r\nContent-Length: " . strlen($response) . "\r\n\r\n" . $response;

$ctx = stream_context_create(['http' => [
    'header' => "Authorization: Bearer SECRET\r\nCookie: sid=abc\r\nX-Custom: keep-me",
]]);

function call(string $location, $ctx): void {
    $client = new SoapClient(null, [
        'location' => $location,
        'uri' => 'test-uri',
        'login' => 'user',
        'password' => 'secret',
        'stream_context' => $ctx,
        'keep_alive' => false,
    ]);
    var_dump($client->__soapCall('foo', []));
}

/* B is on another port than A, so the hop from A to B is cross-origin; B then
 * redirects to itself, credentials must stay withheld for that hop too */
['pid' => $pidB, 'uri' => $uriB] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: /second\r\nContent-Length: 0\r\n\r\n",
    $ok,
], $captureB);
['pid' => $pidA, 'uri' => $uriA] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: $uriB/first\r\nContent-Length: 0\r\n\r\n",
], $captureA);

call("$uriA/src", $ctx);

http_server_kill($pidA);
http_server_kill($pidB);

rewind($captureA);
rewind($captureB);
report('--- origin A (1 request) ---', stream_get_contents($captureA));
report('--- origin B (2 requests) ---', stream_get_contents($captureB));

/* Same origin throughout: credentials are sent on both hops */
['pid' => $pidC, 'uri' => $uriC] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: /next\r\nContent-Length: 0\r\n\r\n",
    $ok,
], $captureC);

call("$uriC/first", $ctx);

http_server_kill($pidC);

rewind($captureC);
report('--- origin C (2 requests) ---', stream_get_contents($captureC));
?>
--EXPECT--
string(2) "ok"
--- origin A (1 request) ---
  Authorization: 1
  Cookie: 1
  X-Custom: 1
--- origin B (2 requests) ---
  Authorization: 0
  Cookie: 0
  X-Custom: 2
string(2) "ok"
--- origin C (2 requests) ---
  Authorization: 2
  Cookie: 2
  X-Custom: 2
