--TEST--
CURLOPT_OPENSOCKETFUNCTION reports an abstract UNIX socket name with its leading NUL byte
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Linux') die('skip abstract UNIX sockets are Linux only');
if (!defined('CURLOPT_ABSTRACT_UNIX_SOCKET')) die('skip CURLOPT_ABSTRACT_UNIX_SOCKET not available');
?>
--FILE--
<?php
/* The leading NUL byte is part of an abstract name: keeping it tells a policy
 * that the name is not a filesystem path. */
$name = 'php-curl-' . getmypid();

$ch = curl_init('http://localhost/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_ABSTRACT_UNIX_SOCKET, $name);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port) use ($name): bool {
    var_dump($address === "\0" . $name, $port);
    return false;
});
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);
?>
--EXPECT--
bool(true)
int(0)
bool(false)
bool(true)
