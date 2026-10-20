--TEST--
GHSA-fpwc-w8rq-cr92: stripping the last user header must not cut the redirected request short
--INI--
allow_url_fopen=1
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--FILE--
<?php
require 'server.inc';

/* Authorization is the last user header and has no trailing line break. When it
 * is stripped on the cross-origin hop, the line break in front of it must go too,
 * otherwise the header block ends early and the body of the 307 POST is lost. */
$ctx = stream_context_create(['http' => [
    'method' => 'POST',
    'header' => "X-Test: 1\r\nContent-Type: text/plain\r\nAuthorization: Basic Zm9vOmJhcg==",
    'content' => 'hello=world',
    'follow_location' => 1,
]]);

$captureB = null;
['pid' => $pidB, 'uri' => $uriB] = http_server([
    "data://text/plain,HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK",
], $captureB);

$captureA = null;
['pid' => $pidA, 'uri' => $uriA] = http_server([
    "data://text/plain,HTTP/1.1 307 Temporary Redirect\r\nLocation: $uriB/target\r\nContent-Length: 0\r\n\r\n",
], $captureA);

var_dump(file_get_contents($uriA . '/start', false, $ctx));

http_server_kill($pidA);
http_server_kill($pidB);

rewind($captureA);
echo "--- origin A ---\n", stream_get_contents($captureA), "\n";
rewind($captureB);
echo "--- origin B ---\n", stream_get_contents($captureB), "\n";
?>
--EXPECTF--
string(2) "OK"
--- origin A ---
POST /start HTTP/1.1
Host: %s:%d
Connection: close
Content-Length: 11
X-Test: 1
Content-Type: text/plain
Authorization: Basic Zm9vOmJhcg==

hello=world
--- origin B ---
POST /target HTTP/1.1
Host: %s:%d
Connection: close
Content-Length: 11
X-Test: 1
Content-Type: text/plain

hello=world
