--TEST--
openssl_get_md_methods() lists only digests that can be used
--EXTENSIONS--
openssl
--FILE--
<?php

$plain = openssl_get_md_methods();
$aliases = openssl_get_md_methods(true);

// Every listed name must be usable. SHAKE has no default output length with OpenSSL >= 3.4.
foreach ([$plain, $aliases] as $methods) {
    foreach ($methods as $method) {
        if (str_starts_with($method, 'shake')) {
            continue;
        }
        if (openssl_digest('abc', $method) === false) {
            var_dump($method);
        }
    }
}

// Names are kept as before and aliases are a superset of the plain list.
var_dump(in_array('sha256', $plain));
var_dump(in_array('sha512-256', $plain));
var_dump(array_diff($plain, $aliases));
// Aliases that resolve to an available digest are still listed.
var_dump(in_array('RSA-SHA256', $aliases));
var_dump(in_array('sha256WithRSAEncryption', $aliases));

// Listing must not leave errors in the error queue.
while (openssl_error_string() !== false);
openssl_get_md_methods(true);
var_dump(openssl_error_string());

?>
--EXPECT--
bool(true)
bool(true)
array(0) {
}
bool(true)
bool(true)
bool(false)
