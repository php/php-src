--TEST--
openssl_x509_checkpurpose(): $x509_verify_flags (CRL check, partial chain)
--EXTENSIONS--
openssl
--FILE--
<?php
/* Fixtures were generated once with a 100 year validity period:
 *   root -> intermediate -> leaf_valid / leaf_revoked
 * with a CRL issued by the intermediate that revokes leaf_revoked only.
 * The CRL is passed as one of the $ca_info files: X509_LOOKUP_load_file()
 * loads every PEM object in a file, certificates and CRLs alike.
 */
$prefix = __DIR__ . "/openssl_x509_checkpurpose_verify_flags_";
$root = $prefix . "root.pem";
$inter = $prefix . "inter.pem";
$crl = $prefix . "inter_crl.pem";
$valid = "file://" . $prefix . "leaf_valid.pem";
$revoked = "file://" . $prefix . "leaf_revoked.pem";
$purpose = X509_PURPOSE_SSL_CLIENT;

echo "Partial chain: only the intermediate is trusted\n";
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$inter]));
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$inter], null, OPENSSL_X509_VERIFY_FLAG_PARTIAL_CHAIN));

echo "Full chain trusted: unaffected by the flag\n";
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$root], $inter));
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$root], $inter, OPENSSL_X509_VERIFY_FLAG_PARTIAL_CHAIN));

echo "No CRL check: revocation is ignored\n";
var_dump(openssl_x509_checkpurpose($revoked, $purpose, [$root, $inter, $crl]));

echo "CRL_CHECK\n";
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$root, $inter, $crl], null, OPENSSL_X509_VERIFY_FLAG_CRL_CHECK));
var_dump(openssl_x509_checkpurpose($revoked, $purpose, [$root, $inter, $crl], null, OPENSSL_X509_VERIFY_FLAG_CRL_CHECK));

echo "CRL_CHECK without a CRL available\n";
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$root, $inter], null, OPENSSL_X509_VERIFY_FLAG_CRL_CHECK));

echo "CRL_CHECK | PARTIAL_CHAIN\n";
var_dump(openssl_x509_checkpurpose($revoked, $purpose, [$inter, $crl], null,
    OPENSSL_X509_VERIFY_FLAG_CRL_CHECK | OPENSSL_X509_VERIFY_FLAG_PARTIAL_CHAIN));

echo "CRL_CHECK_ALL: the root CRL is missing\n";
var_dump(openssl_x509_checkpurpose($valid, $purpose, [$root, $inter, $crl], null,
    OPENSSL_X509_VERIFY_FLAG_CRL_CHECK | OPENSSL_X509_VERIFY_FLAG_CRL_CHECK_ALL));

echo "OpenSSLCertificate object\n";
var_dump(openssl_x509_checkpurpose(openssl_x509_read($valid), $purpose, [$inter], null,
    OPENSSL_X509_VERIFY_FLAG_PARTIAL_CHAIN));

foreach ([0x1000, -1, PHP_INT_MAX] as $flags) {
    try {
        openssl_x509_checkpurpose($valid, $purpose, [$inter], null, $flags);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Partial chain: only the intermediate is trusted
bool(false)
bool(true)
Full chain trusted: unaffected by the flag
bool(true)
bool(true)
No CRL check: revocation is ignored
bool(true)
CRL_CHECK
bool(true)
bool(false)
CRL_CHECK without a CRL available
bool(false)
CRL_CHECK | PARTIAL_CHAIN
bool(false)
CRL_CHECK_ALL: the root CRL is missing
bool(false)
OpenSSLCertificate object
bool(true)
openssl_x509_checkpurpose(): Argument #5 ($x509_verify_flags) must be a combination of supported flags
openssl_x509_checkpurpose(): Argument #5 ($x509_verify_flags) must be a combination of supported flags
openssl_x509_checkpurpose(): Argument #5 ($x509_verify_flags) must be a combination of supported flags
