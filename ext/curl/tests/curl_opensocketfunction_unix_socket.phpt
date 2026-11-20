--TEST--
CURLOPT_OPENSOCKETFUNCTION reports the path and port 0 for a UNIX socket
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_UNIX_SOCKET_PATH')) die('skip CURLOPT_UNIX_SOCKET_PATH not available');
if (PHP_OS_FAMILY === 'Windows') die('skip UNIX domain sockets');
?>
--FILE--
<?php
/* A path has no port: $port is 0, as CURLINFO_PRIMARY_PORT reports. The path is
 * kept short, as sun_path holds 104 bytes on some systems; no socket is
 * created, since the callback refuses it. */
$path = sys_get_temp_dir() . '/php-curl-' . getmypid() . '.sock';
if (strlen($path) > 100) {
    $path = '/tmp/php-curl-' . getmypid() . '.sock';
}

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, 'http://localhost/');
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_UNIX_SOCKET_PATH, $path);
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port) use ($path): bool {
    var_dump($address === $path, $port);
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
