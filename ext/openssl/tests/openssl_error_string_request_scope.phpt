--TEST--
openssl_error_string() does not return errors from a previous request
--EXTENSIONS--
openssl
--CONFLICTS--
server
--FILE--
<?php
include __DIR__ . '/../../../sapi/cli/tests/php_cli_server.inc';

$config = __DIR__ . '/openssl_error_string_request_scope.cnf';
$randfile = __DIR__ . '/openssl_error_string_request_scope.missing';
file_put_contents($config, "RANDFILE = $randfile\n[ req ]\ndistinguished_name = dn\n[ dn ]\n");

$code = <<<PHP
if (isset(\$_GET['stored'])) {
    // The error is moved to the extension's own queue
    @openssl_x509_read('not a certificate');
    echo "stored\n";
} elseif (isset(\$_GET['library'])) {
    // The missing RANDFILE leaves an error in OpenSSL's queue only
    openssl_pkey_new(['config' => '$config', 'private_key_type' => OPENSSL_KEYTYPE_EC, 'curve_name' => 'prime256v1']);
    echo "library\n";
} else {
    var_dump(openssl_error_string());
}
PHP;

$args = ['-d', 'extension_dir=' . ini_get('extension_dir')];
$args[] = '-d';
$args[] = 'extension=' . (PHP_OS_FAMILY === 'Windows' ? 'php_' : '') . 'openssl.' . PHP_SHLIB_SUFFIX;
if (php_ini_loaded_file()) {
    $args[] = '-c';
    $args[] = php_ini_loaded_file();
}
php_cli_server_start($code, 'index.php', $args);

foreach (['stored', 'library'] as $case) {
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . "/?$case=1");
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/');
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/openssl_error_string_request_scope.cnf');
?>
--EXPECT--
stored
bool(false)
library
bool(false)
