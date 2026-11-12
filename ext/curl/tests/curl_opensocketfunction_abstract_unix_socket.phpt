--TEST--
CURLOPT_OPENSOCKETFUNCTION: abstract UNIX domain socket
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip Linux only');
if (!(curl_version()['features'] & CURL_VERSION_UNIX_SOCKETS)) die('skip libcurl without UNIX domain sockets');
?>
--FILE--
<?php
$ch = curl_init('http://localhost/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_ABSTRACT_UNIX_SOCKET, 'php-curl-does-not-exist');
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port): bool {
    /* The address keeps the leading NUL byte of abstract sockets */
    var_dump($address === "\0php-curl-does-not-exist", $port);
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
