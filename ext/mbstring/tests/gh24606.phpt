--TEST--
GH-24606 (mbstring.http_input is ignored in subsequent requests after changing default_charset)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = ["-d", "extension_dir=" . ini_get("extension_dir"), "-d", "extension=" . (substr(PHP_OS, 0, 3) == "WIN" ? "php_" : "") . "mbstring." . PHP_SHLIB_SUFFIX];
if (php_ini_loaded_file()) {
    $args[] = "-c";
    $args[] = php_ini_loaded_file();
}
$args = [...$args, "-d", "default_charset=UTF-8", "-d", "mbstring.http_input=ISO-8859-1", "-d", "error_reporting=E_ALL&~E_DEPRECATED"];

$code = <<<'PHP'
echo implode(",", mb_http_input("I")), "\n";
if (isset($_GET["change"])) {
    ini_set("default_charset", "SJIS");
}
PHP;

php_cli_server_start($code, null, $args);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/?change=1");
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/");
?>
--EXPECT--
ISO-8859-1
ISO-8859-1
