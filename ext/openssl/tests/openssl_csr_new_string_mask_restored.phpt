--TEST--
openssl_csr_new(): string_mask of a config does not apply to later calls
--EXTENSIONS--
openssl
--FILE--
<?php
$dir = __DIR__;
$set = "$dir/openssl_csr_new_string_mask_restored_set.cnf";
$plain = "$dir/openssl_csr_new_string_mask_restored_plain.cnf";
file_put_contents($set, "[ req ]\ndistinguished_name = dn\nstring_mask = nombstr\n[ dn ]\n");
file_put_contents($plain, "[ req ]\ndistinguished_name = dn\n[ dn ]\n");

function csr(string $config, string $cn): bool {
    $opts = ['config' => $config];
    $key = openssl_pkey_new(['private_key_type' => OPENSSL_KEYTYPE_EC, 'curve_name' => 'prime256v1'] + $opts);
    return @openssl_csr_new(['commonName' => $cn], $key, $opts) !== false;
}

var_dump(csr($plain, 'Łódź'));
var_dump(csr($set, 'example'));
var_dump(csr($set, 'Łódź'));
var_dump(csr($plain, 'Łódź'));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/openssl_csr_new_string_mask_restored_set.cnf');
@unlink(__DIR__ . '/openssl_csr_new_string_mask_restored_plain.cnf');
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
