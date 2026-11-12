--TEST--
CURLOPT_OPENSOCKETFUNCTION: UNIX domain socket
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip not for Windows');
if (!(curl_version()['features'] & CURL_VERSION_UNIX_SOCKETS)) die('skip libcurl without UNIX domain sockets');
?>
--FILE--
<?php
$ch = curl_init('http://localhost/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_UNIX_SOCKET_PATH, '/does-not-exist/php-curl.sock');
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port): bool {
    var_dump($address === '/does-not-exist/php-curl.sock', $port);
    return false;
});
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "Done";
?>
--EXPECT--
bool(true)
int(0)
bool(false)
bool(true)
Done
