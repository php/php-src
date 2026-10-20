--TEST--
GHSA-fpwc-w8rq-cr92: strip credentials from user headers on cross-origin redirect
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

function count_header(string $requests, string $name): int {
    return preg_match_all('/^' . preg_quote($name, '/') . ':/mi', $requests);
}

function report(string $label, string $requests): void {
    echo $label, "\n";
    foreach (['Authorization', 'Cookie', 'Proxy-Authorization', 'X-Custom'] as $name) {
        echo "  $name: ", count_header($requests, $name), "\n";
    }
}

$ctx = stream_context_create(['http' => [
    'header' => "Authorization: Bearer SECRET\r\n"
        . "Cookie: sid=abc\r\n"
        . "Proxy-Authorization: Basic Zm9vOmJhcg==\r\n"
        . "X-Custom: keep-me",
    'follow_location' => 1,
]]);

/* server B listens on a different port than server A, so the hop from A to B is
 * cross-origin; B then redirects to itself: credentials must stay withheld for
 * that same-origin hop too */
$captureB = null;
['pid' => $pidB, 'uri' => $uriB] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: /second\r\nContent-Length: 0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
], $captureB);

$captureA = null;
['pid' => $pidA, 'uri' => $uriA] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: $uriB/first\r\nContent-Length: 0\r\n\r\n",
], $captureA);

var_dump(file_get_contents($uriA . '/src', false, $ctx));

http_server_kill($pidA);
http_server_kill($pidB);

rewind($captureA);
rewind($captureB);
report('--- origin A (1 request) ---', stream_get_contents($captureA));
report('--- origin B (2 requests) ---', stream_get_contents($captureB));

/* same origin throughout: credentials must be sent on both hops */
$captureC = null;
['pid' => $pidC, 'uri' => $uriC] = http_server([
    "data://text/plain,HTTP/1.1 302 Found\r\nLocation: /next\r\nContent-Length: 0\r\n\r\n",
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
], $captureC);

var_dump(file_get_contents($uriC . '/first', false, $ctx));

http_server_kill($pidC);

rewind($captureC);
report('--- origin C (2 requests) ---', stream_get_contents($captureC));
?>
--EXPECT--
string(2) "OK"
--- origin A (1 request) ---
  Authorization: 1
  Cookie: 1
  Proxy-Authorization: 1
  X-Custom: 1
--- origin B (2 requests) ---
  Authorization: 0
  Cookie: 0
  Proxy-Authorization: 0
  X-Custom: 2
string(2) "OK"
--- origin C (2 requests) ---
  Authorization: 2
  Cookie: 2
  Proxy-Authorization: 2
  X-Custom: 2
