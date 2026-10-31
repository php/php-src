--TEST--
GH-25093 (Phar::webPhar() builds the redirect target from the on-disk path length)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = [];
if (php_ini_loaded_file()) {
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}
// The server is started with -n, so a shared build must be told to load phar.
$module = (substr(PHP_OS, 0, 3) === "WIN" ? "php_" : "") . "phar." . PHP_SHLIB_SUFFIX;
if (file_exists(ini_get("extension_dir") . DIRECTORY_SEPARATOR . $module)) {
    $args[] = "-d";
    $args[] = "extension_dir=" . ini_get("extension_dir");
    $args[] = "-d";
    $args[] = "extension=" . $module;
}
$server = php_cli_server_start('require __DIR__ . "/sub/app.phar";', "router.php", $args);

@mkdir("$server->docRoot/sub");
@unlink("$server->docRoot/sub/app.phar");
$phar = new Phar("$server->docRoot/sub/app.phar");
$phar["index.php"] = "<?php echo 'index';";
$phar->setStub('<?php Phar::webPhar(null, "/index.php", null, [], function ($uri) { return "/"; }); __HALT_COMPILER(); ?>');
unset($phar);

function request_redirect(string $uri): void
{
    $client = stream_socket_client("tcp://" . PHP_CLI_SERVER_ADDRESS);
    fwrite($client, "GET $uri HTTP/1.1\r\nHost: " . PHP_CLI_SERVER_HOSTNAME . "\r\nConnection: close\r\n\r\n");
    $response = stream_get_contents($client);
    fclose($client);

    foreach (explode("\r\n", $response) as $line) {
        if ($line === "") {
            break;
        }
        if (str_starts_with($line, "HTTP/") || str_starts_with($line, "Location:")) {
            echo $line, "\n";
        }
    }
}

// Requested path longer than the phar's path on disk: the redirect target kept
// part of the requested path.
request_redirect("/sub/app.phar/" . str_repeat("a", strlen($server->docRoot) + 16));

// Requested path ending at the phar: the redirect target was measured past the
// end of the request URI.
request_redirect("/sub/app.phar");
?>
--CLEAN--
<?php
$root = __DIR__ . "/../../../sapi/cli/tests/gh25093";
@unlink("$root/sub/app.phar");
@rmdir("$root/sub");
@unlink("$root/router.php");
@rmdir($root);
?>
--EXPECT--
HTTP/1.1 301 Moved Permanently
Location: /sub/app.phar/index.php
HTTP/1.1 301 Moved Permanently
Location: /sub/app.phar/index.php
