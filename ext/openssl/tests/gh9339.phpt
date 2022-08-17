--TEST--
GH-9339: oid_file path check warning contains uninitialized path
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip openssl not loaded");
?>
--FILE--
<?php
$configCode = <<<CONFIG
oid_file = %s
[ req ]
default_bits = 1024
CONFIG;

$configFile = __DIR__ . '/gh9339.cnf';
file_put_contents($configFile, sprintf($configCode,  __DIR__ . '/' . str_repeat('a', 9000)));
openssl_pkey_new([ 'config' => $configFile ]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh9339.cnf');
?>
--EXPECTF--

Warning: openssl_pkey_new(): Path for oid_file option must be a valid file path in %s on line %d
