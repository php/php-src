--TEST--
GH-25093: Phar::webPhar() redirect uses the full phar path length to cut the request URI
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

$info = php_cli_server_start('include __DIR__ . "/gh25093.phar";', 'router.php');

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

foreach ($http_response_header as $header) {
    if (str_starts_with($header, 'Location: ')) {
        echo $header, "\n";
    }
}
?>
--EXPECT--
Location: /gh25093.phar/index.php
