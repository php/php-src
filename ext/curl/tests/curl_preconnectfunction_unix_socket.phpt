--TEST--
CURLOPT_PRECONNECTFUNCTION reports a null address for a UNIX socket
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_UNIX_SOCKET_PATH')) die('skip CURLOPT_UNIX_SOCKET_PATH not available');
if (PHP_OS_FAMILY === 'Windows') die('skip UNIX domain sockets');
?>
--FILE--
<?php
/* CURLOPT_UNIX_SOCKET_PATH has no address to report, so $ip is null rather than an
 * empty string: a policy has to handle the case instead of matching against ''. */
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, 'http://localhost/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_UNIX_SOCKET_PATH, __DIR__ . '/does-not-exist.sock');
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle, ?string $ip, int $port, CurlAddressFamily $family): bool {
    var_dump($ip, $port, $family);
    return false;
});
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "Done";
?>
--EXPECT--
NULL
int(0)
enum(CurlAddressFamily::Unix)
bool(false)
bool(true)
Done
