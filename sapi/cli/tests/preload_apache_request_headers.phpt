--TEST--
apache_request_headers() does not crash the built-in server when called from opcache.preload
--SKIPIF--
<?php
include "skipif.inc";
$extDir = ini_get('extension_dir');
if (!file_exists($extDir . '/opcache.so') && !file_exists($extDir . '/opcache.dll')) {
    die('skip opcache shared object not found in extension_dir');
}
?>
--FILE--
<?php
include "php_cli_server.inc";

// A preload script runs during server startup, before any request, so
// SG(server_context) is NULL. apache_request_headers() used to dereference
// it unconditionally and crash the server before it could bind.
$preload_file = __DIR__ . DIRECTORY_SEPARATOR . basename(__FILE__, '.phpt') . '-preload.php';
file_put_contents($preload_file, '<?php apache_request_headers();');
register_shutdown_function(fn() => @unlink($preload_file));

php_cli_server_start('echo "alive";', 'index.php', [
    '-d', 'extension_dir=' . ini_get('extension_dir'),
    '-d', 'extension=opcache',
    '-d', 'opcache.enable_cli=1',
    '-d', 'opcache.preload=' . $preload_file,
]);

$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nHost: " . PHP_CLI_SERVER_HOSTNAME . "\r\n\r\n");
fflush($fp);
while (!feof($fp)) {
    echo fgets($fp);
}
fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
%a
alive
