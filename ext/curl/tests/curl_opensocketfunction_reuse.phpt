--TEST--
CURLOPT_OPENSOCKETFUNCTION is not invoked for a connection reused from the pool
--EXTENSIONS--
curl
--FILE--
<?php
include 'keepalive_server.inc';

/* The callback is invoked when a socket is created. A connection taken from
 * libcurl's pool creates none, so the callback only vets connections opened
 * while it is set. The server answers with the number of connections it has
 * accepted, which shows whether a request was sent on a new connection. */
$url = curl_keepalive_server_start() . '/';
$calls = 0;
$count = function (CurlHandle $handle, string $address, int $port) use (&$calls): bool {
    $calls++;
    return true;
};

echo "Set before the first transfer: only the new connection is vetted\n";
$ch = curl_init($url);
curl_setopt_array($ch, [CURLOPT_RETURNTRANSFER => true, CURLOPT_PROXY => '']);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, $count);
var_dump(curl_exec($ch), curl_exec($ch), $calls);

echo "\nSet after a transfer: the pooled connection is reused without the callback\n";
$calls = 0;
$ch = curl_init($url);
curl_setopt_array($ch, [CURLOPT_RETURNTRANSFER => true, CURLOPT_PROXY => '']);
var_dump(curl_exec($ch));
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, $count);
var_dump(curl_exec($ch), $calls);

echo "\nCURLOPT_FRESH_CONNECT opens a new connection, which is vetted\n";
curl_setopt($ch, CURLOPT_FRESH_CONNECT, true);
var_dump(curl_exec($ch), $calls);

echo "\nCURLOPT_PREREQFUNCTION runs before every request, on a reused connection too\n";
curl_setopt($ch, CURLOPT_FRESH_CONNECT, false);
curl_setopt($ch, CURLOPT_PREREQFUNCTION, function (CurlHandle $handle, string $ip, string $localIp, int $port, int $localPort): int {
    var_dump($ip);
    return CURL_PREREQFUNC_OK;
});
var_dump(curl_exec($ch), $calls);

echo "Done\n";
?>
--EXPECT--
Set before the first transfer: only the new connection is vetted
string(1) "1"
string(1) "1"
int(1)

Set after a transfer: the pooled connection is reused without the callback
string(1) "2"
string(1) "2"
int(0)

CURLOPT_FRESH_CONNECT opens a new connection, which is vetted
string(1) "3"
int(1)

CURLOPT_PREREQFUNCTION runs before every request, on a reused connection too
string(9) "127.0.0.1"
string(1) "3"
int(1)
Done
