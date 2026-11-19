--TEST--
GH-9317: OpenSSL config path does not respect open_basedir restriction
--EXTENSIONS--
openssl
--FILE--
<?php
$baseDir = __DIR__ . DIRECTORY_SEPARATOR . 'gh9317';
@mkdir($baseDir);
$baseDirConfig = $baseDir . DIRECTORY_SEPARATOR . 'openssl.cnf';
copy(__DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf', $baseDirConfig);

ini_set('open_basedir', $baseDir);

$args = [
    'private_key_type' => OPENSSL_KEYTYPE_EC,
    'curve_name' => 'prime256v1',
];

echo "Config outside of open_basedir:\n";
var_dump(openssl_pkey_new($args + ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf']));

echo "Config with a null byte:\n";
var_dump(openssl_pkey_new($args + ['config' => $baseDirConfig . "\0.cnf"]));

echo "Config inside of open_basedir:\n";
var_dump(openssl_pkey_new($args + ['config' => $baseDirConfig]) instanceof OpenSSLAsymmetricKey);

echo "Default config:\n";
var_dump(openssl_pkey_new($args) instanceof OpenSSLAsymmetricKey);
?>
--CLEAN--
<?php
$baseDir = __DIR__ . DIRECTORY_SEPARATOR . 'gh9317';
@unlink($baseDir . DIRECTORY_SEPARATOR . 'openssl.cnf');
@rmdir($baseDir);
?>
--EXPECTF--
Config outside of open_basedir:

Warning: openssl_pkey_new(): open_basedir restriction in effect. File(%sopenssl.cnf) is not within the allowed path(s): (%sgh9317) in %s on line %d
bool(false)
Config with a null byte:

Warning: openssl_pkey_new(): Path for config array item must not contain any null bytes in %s on line %d
bool(false)
Config inside of open_basedir:
bool(true)
Default config:
bool(true)
