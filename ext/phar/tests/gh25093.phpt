--TEST--
GH-25093: Phar::webPhar() redirect reads past the request URI
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--CONFLICTS--
server
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "phar." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
  $args[] = "-c";
  $args[] = php_ini_loaded_file();
}
$info = php_cli_server_start('include __DIR__ . "/gh25093.phar";', 'router.php', $args);

$phar = new Phar($info->docRoot . '/gh25093.phar');
$phar->addFromString('index.php', '<?php echo "index\n";');
$phar->setStub('<?php
Phar::webPhar(null, "index.php", null, [], function ($path) {
    return "/";
});
__HALT_COMPILER(); ?>');
unset($phar);

$context = stream_context_create(['http' => ['follow_location' => 0]]);
$uri = '/gh25093.phar/' . str_repeat('a', strlen($info->docRoot) + 32);
file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . $uri, false, $context);

foreach (http_get_last_response_headers() as $header) {
    if (str_starts_with($header, 'Location: ')) {
        echo $header, "\n";
    }
}
?>
--EXPECT--
Location: /gh25093.phar/index.php
