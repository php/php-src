--TEST--
SOAP error handler keeps a custom HTTP status line on a non-fatal error
--EXTENSIONS--
soap
--SKIPIF--
<?php
    if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
        echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
    }
?>
--FILE--
<?php

include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "soap." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
  // Necessary such that it works from a development directory in which case extension_dir might not be the real extension dir
  $args[] = "-c";
  $args[] = php_ini_loaded_file();
}
$code = <<<'PHP'
header('HTTP/1.1 404 Custom Reason');
use_soap_error_handler(true);
echo $undefinedVar;
echo "plain";
PHP;

php_cli_server_start($code, null, $args);

$body = file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS, false, stream_context_create(['http' => ['ignore_errors' => true]]));
echo $http_response_header[0], "\n", $body, "\n";
?>
--EXPECT--
HTTP/1.1 404 Custom Reason
plain
