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

/* a stripped header that was last in the bag must not leave a trailing line break
 * behind, or the request body would be pushed out of the request */
$ctx = stream_context_create(['http' => [
    'method' => 'POST',
    'content' => 'hello=world',
    'header' => "X-Custom: keep-me\r\nAuthorization: Bearer SECRET",
    'follow_location' => 1,
]]);

$captureH = null;
['pid' => $pidH, 'uri' => $uriH] = http_server([
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
], $captureH);

$captureG = null;
['pid' => $pidG, 'uri' => $uriG] = http_server([
    "data://text/plain,HTTP/1.1 307 Temporary Redirect\r\nLocation: $uriH/second\r\nContent-Length: 0\r\n\r\n",
], $captureG);

var_dump(@file_get_contents($uriG . '/first', false, $ctx));

http_server_kill($pidG);
http_server_kill($pidH);

rewind($captureH);
echo "--- credential header last in the bag (307) ---\n";
echo preg_replace('/^Host:.*$/m', 'Host: ...', stream_get_contents($captureH));

echo "--- malformed header bags ---\n";
foreach ([
    'folded value      ' => "Authorization:\r\n Bearer SECRET\r\nX-Custom: keep-me",
    'folded value (tab)' => "Authorization:\r\n\tBearer SECRET\r\nX-Custom: keep-me",
    'lone CR           ' => "X-Custom: keep-me\rAuthorization: Bearer SECRET",
    'space before colon' => "Authorization : Bearer SECRET\r\nX-Custom: keep-me",
    'tab before colon  ' => "Authorization\t: Bearer SECRET\r\nX-Custom: keep-me",
] as $label => $header) {
    $ctx = stream_context_create(['http' => ['header' => $header, 'follow_location' => 1]]);

    $captureF = null;
    ['pid' => $pidF, 'uri' => $uriF] = http_server([
        "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
    ], $captureF);

    $captureE = null;
    ['pid' => $pidE, 'uri' => $uriE] = http_server([
        "data://text/plain,HTTP/1.1 302 Found\r\nLocation: $uriF/second\r\nContent-Length: 0\r\n\r\n",
    ], $captureE);

    file_get_contents($uriE . '/first', false, $ctx);

    http_server_kill($pidE);
    http_server_kill($pidF);

    rewind($captureF);
    $request = stream_get_contents($captureF);
    printf("  %s  SECRET leaked: %d, X-Custom kept: %d\n", $label,
        str_contains($request, 'SECRET'), str_contains($request, 'X-Custom'));
}
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
string(2) "OK"
--- credential header last in the bag (307) ---
POST /second HTTP/1.1
Host: ...
Connection: close
Content-Length: 11
X-Custom: keep-me
Content-Type: application/x-www-form-urlencoded

hello=world--- malformed header bags ---
  folded value        SECRET leaked: 0, X-Custom kept: 1
  folded value (tab)  SECRET leaked: 0, X-Custom kept: 1
  lone CR             SECRET leaked: 0, X-Custom kept: 1
  space before colon  SECRET leaked: 0, X-Custom kept: 1
  tab before colon    SECRET leaked: 0, X-Custom kept: 1
