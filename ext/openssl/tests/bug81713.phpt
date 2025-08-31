--TEST--
Bug #81713 (OpenSSL functions null byte injection)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
--FILE--
<?php
$priv_key_file = "file://" . __DIR__ . "/private_rsa_1024.key";
$priv_key_file_null = "$priv_key_file\x00foo";
$crt =  __DIR__ . '/cert.crt';
$crt_null =  "$crt\x00foo";
$csr = __DIR__ . '/cert.csr';
$crt_file = "file://$crt";
$crt_file_null = "$crt_file\x00foo";
$csr_file = "file://$csr";
$csr_file_null = "$csr_file\x00foo";
$infile = __DIR__ . '/plain.txt';
$infile_null = "$infile\x00acd";;
$outfile = __DIR__ . '/bug81713.out';
$outfile_null = "$outfile\x00acd";
$eml = __DIR__ . "/signed.eml";
$eml_null = "$eml\x00foo";
$headers = ["test@test", "testing openssl_cms_encrypt()"];
$cainfo = [$crt];
$cainfo_null = [$crt_file_null];

$config = __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf';
$config_arg = array('config' => $config);

$dn = [
    "countryName" => "BR",
    "stateOrProvinceName" => "Rio Grande do Sul",
    "localityName" => "Porto Alegre",
    "commonName" => "Henrique do N. Angelo",
    "emailAddress" => "hnangelo@php.net"
];

$csr_args = [
    "digest_alg" => "sha256",
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_DSA,
    "encrypt_key" => true,
    "config" => $config,
];

$tests = [
    ["openssl_pkey_get_public", $crt_file],
    ["openssl_pkey_get_public", $crt_file_null],
    ["openssl_pkey_get_private", $crt_file_null],
    ["openssl_pkey_export_to_file", $priv_key_file_null, $outfile],
    ["openssl_pkey_export_to_file", $priv_key_file, $outfile_null],
    ["openssl_pkey_export", $priv_key_file_null, &$out],
    ["openssl_pkey_derive", $priv_key_file_null, $priv_key_file],
    ["openssl_pkey_derive", $priv_key_file, $priv_key_file_null],
    ["openssl_private_encrypt", "test", &$out, $priv_key_file_null],
    ["openssl_private_decrypt", "test", &$out, $priv_key_file_null],
    ["openssl_public_encrypt", "test", &$out, $priv_key_file_null],
    ["openssl_public_decrypt", "test", &$out, $priv_key_file_null],
    ["openssl_sign", "test", &$out, $priv_key_file_null],
    ["openssl_verify", "test", "sig", $priv_key_file_null],
    ["openssl_seal", "test", &$sealed, &$ekeys, [$priv_key_file_null], "AES-128-CBC", &$iv],
    ["openssl_open", "test", &$open, "aaa", $priv_key_file_null, "AES-128-CBC", &$iv],
    ["openssl_csr_new", $dn, &$priv_key_file_null, $csr_args],
    ["openssl_csr_get_subject", $csr_file_null],
    ["openssl_csr_get_public_key", $csr_file_null],
    ["openssl_x509_fingerprint", $crt_file_null],
    ["openssl_x509_export_to_file", $crt_file_null, $outfile],
    ["openssl_x509_export_to_file", $crt_file, $outfile_null],
    ["openssl_x509_export", $crt_file_null, &$out],
    ["openssl_x509_checkpurpose", $crt_file_null, X509_PURPOSE_SSL_CLIENT],
    ["openssl_x509_checkpurpose", $crt_file, X509_PURPOSE_SSL_CLIENT, $cainfo_null],
    ["openssl_x509_check_private_key", $crt_file_null, $priv_key_file],
    ["openssl_x509_check_private_key", $crt_file, $priv_key_file_null],
    ["openssl_x509_verify", $crt_file_null, $priv_key_file],
    ["openssl_x509_verify", $crt_file, $priv_key_file_null],
    ["openssl_x509_parse", $crt_file_null],
    ["openssl_x509_read", $crt_file_null],
    ["openssl_cms_encrypt", $infile_null, $outfile, $crt_file, $headers],
    ["openssl_cms_encrypt", $infile, $outfile_null, $crt_file, $headers],
    ["openssl_cms_encrypt", $infile, $outfile, $crt_file_null, $headers],
    ["openssl_cms_encrypt", $infile, $outfile, $cainfo_null, $headers],
    ["openssl_cms_decrypt", $infile_null, $outfile,  $crt_file, $priv_key_file],
    ["openssl_cms_decrypt", $infile, $outfile_null, $crt_file, $priv_key_file],
    ["openssl_cms_decrypt", $infile, $outfile, $crt_file_null, $priv_key_file],
    ["openssl_cms_decrypt", $infile, $outfile, $crt_file, $priv_key_file_null],
    ["openssl_cms_sign", $infile_null, "$outfile", $crt_file, $priv_key_file, $headers],
    ["openssl_cms_sign", $infile, $outfile_null, $crt_file, $priv_key_file, $headers],
    ["openssl_cms_sign", $infile, $outfile, $crt_file_null, $priv_key_file, $headers],
    ["openssl_cms_sign", $infile, $outfile, $crt_file, $crt_file_null, $headers],
    ["openssl_cms_sign", $infile, $outfile, $crt_file, $crt_file, $headers, 0, OPENSSL_ENCODING_DER, $crt_file_null],
    ["openssl_cms_verify", $eml_null, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile, $outfile, $outfile],
    ["openssl_cms_verify", $eml, OPENSSL_CMS_NOVERIFY, $outfile_null, $cainfo, $outfile, $outfile, $outfile],
    ["openssl_cms_verify", $eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo_null],
    ["openssl_cms_verify", $eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile_null, $outfile, $outfile],
    ["openssl_cms_verify", $eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile, $outfile_null, $outfile],
    ["openssl_cms_verify", $eml, OPENSSL_CMS_NOVERIFY, $outfile, $cainfo, $outfile, $outfile, $outfile_null],
    ["openssl_pkcs7_encrypt", $infile_null, $outfile, $crt_file, $headers],
    ["openssl_pkcs7_encrypt", $infile, $outfile_null, $crt_file, $headers],
    ["openssl_pkcs7_encrypt", $infile, $outfile, $crt_file_null, $headers],
    ["openssl_pkcs7_encrypt", $infile, $outfile, $cainfo_null, $headers],
    ["openssl_pkcs7_decrypt", $infile_null, $outfile,  $crt_file, $priv_key_file],
    ["openssl_pkcs7_decrypt", $infile, $outfile_null, $crt_file, $priv_key_file],
    ["openssl_pkcs7_decrypt", $infile, $outfile, $crt_file_null, $priv_key_file],
    ["openssl_pkcs7_decrypt", $infile, $outfile, $crt_file, $priv_key_file_null],
    ["openssl_pkcs7_sign", $infile_null, "$outfile", $crt_file, $priv_key_file, $headers],
    ["openssl_pkcs7_sign", $infile, $outfile_null, $crt_file, $priv_key_file, $headers],
    ["openssl_pkcs7_sign", $infile, $outfile, $crt_file_null, $priv_key_file, $headers],
    ["openssl_pkcs7_sign", $infile, $outfile, $crt_file, $crt_file_null, $headers],
    ["openssl_pkcs7_sign", $infile, $outfile, $crt_file, $crt_file, $headers, 0, $crt_file_null],
    ["openssl_pkcs7_verify", $eml_null, 0, $outfile, $cainfo, $outfile, $outfile, $outfile],
    ["openssl_pkcs7_verify", $eml, 0, $outfile_null, $cainfo, $outfile, $outfile, $outfile],
    ["openssl_pkcs7_verify", $eml, 0, $outfile, $cainfo_null],
    ["openssl_pkcs7_verify", $eml, 0, $outfile, $cainfo, $outfile_null, $outfile, $outfile],
    ["openssl_pkcs7_verify", $eml, 0, $outfile, $cainfo, $outfile, $outfile_null, $outfile],
    ["openssl_pkcs7_verify", $eml, 0, $outfile, $cainfo, $outfile, $outfile, $outfile_null],
    ["openssl_pkcs12_export", $crt_file_null, &$out, $priv_key_file, "pwd"],
    ["openssl_pkcs12_export", $crt_file, &$out, $priv_key_file_null, "pwd"],
    ["openssl_pkcs12_export", $crt_file, &$out, $priv_key_file, "pwd", ["extracerts" => [$crt_file_null]]],
    ["openssl_pkcs12_export_to_file", $crt_file_null, $outfile, $priv_key_file, "pwd"],
    ["openssl_pkcs12_export_to_file", $crt_file, $outfile_null, $priv_key_file_null, "pwd"],
    ["openssl_pkcs12_export_to_file", $crt_file, $outfile, $priv_key_file_null, "pwd"],
    ["openssl_pkcs12_export_to_file", $crt_file, $outfile, $priv_key_file, "pwd", ["extracerts" => [$crt_file_null]]],
];
foreach ($tests as $test) {
    try {
        $key = call_user_func_array($test[0], array_slice($test, 1));
        var_dump($key);
    }
    catch (ValueError $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}
?>
--CLEAN--
<?php
$outfile = __DIR__ . '/bug81713.out';
@unlink($outfile);
?>
--EXPECTF--
object(OpenSSLAsymmetricKey)#1 (0) {
}
openssl_pkey_get_public(): Argument #1 ($public_key) must not contain any null bytes
openssl_pkey_get_private(): Argument #1 ($private_key) must not contain any null bytes
openssl_pkey_export_to_file(): Argument #1 ($key) must not contain any null bytes
openssl_pkey_export_to_file(): Argument #2 ($output_filename) must not contain any null bytes
openssl_pkey_export(): Argument #1 ($key) must not contain any null bytes
openssl_pkey_derive(): Argument #1 ($public_key) must not contain any null bytes
openssl_pkey_derive(): Argument #2 ($private_key) must not contain any null bytes
openssl_private_encrypt(): Argument #3 ($private_key) must not contain any null bytes
openssl_private_decrypt(): Argument #3 ($private_key) must not contain any null bytes
openssl_public_encrypt(): Argument #3 ($public_key) must not contain any null bytes
openssl_public_decrypt(): Argument #3 ($public_key) must not contain any null bytes
openssl_sign(): Argument #3 ($private_key) must not contain any null bytes
openssl_verify(): Argument #3 ($public_key) must not contain any null bytes
openssl_seal(): Argument #4 ($public_key) must not contain any null bytes
openssl_open(): Argument #4 ($private_key) must not contain any null bytes
openssl_csr_new(): Argument #2 ($private_key) must not contain any null bytes
openssl_csr_get_subject(): Argument #1 ($csr) must not contain any null bytes
openssl_csr_get_public_key(): Argument #1 ($csr) must not contain any null bytes

Warning: openssl_x509_fingerprint(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_x509_fingerprint(): Argument #1 ($certificate) must not contain any null bytes

Warning: openssl_x509_export_to_file(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_x509_export_to_file(): Argument #1 ($certificate) must not contain any null bytes
openssl_x509_export_to_file(): Argument #2 ($output_filename) must not contain any null bytes

Warning: openssl_x509_export(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_x509_export(): Argument #1 ($certificate) must not contain any null bytes
openssl_x509_checkpurpose(): Argument #1 ($certificate) must not contain any null bytes
openssl_x509_checkpurpose(): Argument #3 ($ca_info) array item must not contain any null bytes
openssl_x509_check_private_key(): Argument #1 ($certificate) must not contain any null bytes
openssl_x509_check_private_key(): Argument #2 ($private_key) must not contain any null bytes
openssl_x509_verify(): Argument #1 ($certificate) must not contain any null bytes
openssl_x509_verify(): Argument #2 ($public_key) must not contain any null bytes
openssl_x509_parse(): Argument #1 ($certificate) must not contain any null bytes

Warning: openssl_x509_read(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_x509_read(): Argument #1 ($certificate) must not contain any null bytes
openssl_cms_encrypt(): Argument #1 ($input_filename) must not contain any null bytes
openssl_cms_encrypt(): Argument #2 ($output_filename) must not contain any null bytes
openssl_cms_encrypt(): Argument #3 ($certificate) must not contain any null bytes
openssl_cms_encrypt(): Argument #3 ($certificate) array item must not contain any null bytes
openssl_cms_decrypt(): Argument #1 ($input_filename) must not contain any null bytes
openssl_cms_decrypt(): Argument #2 ($output_filename) must not contain any null bytes

Warning: openssl_cms_decrypt(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_cms_decrypt(): Argument #3 ($certificate) must not contain any null bytes
openssl_cms_decrypt(): Argument #4 ($private_key) must not contain any null bytes
openssl_cms_sign(): Argument #1 ($input_filename) must not contain any null bytes
openssl_cms_sign(): Argument #2 ($output_filename) must not contain any null bytes

Warning: openssl_cms_sign(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_cms_sign(): Argument #3 ($certificate) must not contain any null bytes
openssl_cms_sign(): Argument #4 ($private_key) must not contain any null bytes
openssl_cms_sign(): Argument #8 ($untrusted_certificates_filename) must not contain any null bytes
openssl_cms_verify(): Argument #1 ($input_filename) must not contain any null bytes
openssl_cms_verify(): Argument #3 ($certificates) must not contain any null bytes
openssl_cms_verify(): Argument #4 ($ca_info) array item must not contain any null bytes
openssl_cms_verify(): Argument #5 ($untrusted_certificates_filename) must not contain any null bytes
openssl_cms_verify(): Argument #6 ($content) must not contain any null bytes
openssl_cms_verify(): Argument #7 ($pk7) must not contain any null bytes
openssl_pkcs7_encrypt(): Argument #1 ($input_filename) must not contain any null bytes
openssl_pkcs7_encrypt(): Argument #2 ($output_filename) must not contain any null bytes
openssl_pkcs7_encrypt(): Argument #3 ($certificate) must not contain any null bytes
openssl_pkcs7_encrypt(): Argument #3 ($certificate) array item must not contain any null bytes
openssl_pkcs7_decrypt(): Argument #1 ($input_filename) must not contain any null bytes
openssl_pkcs7_decrypt(): Argument #2 ($output_filename) must not contain any null bytes

Warning: openssl_pkcs7_decrypt(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_pkcs7_decrypt(): Argument #3 ($certificate) must not contain any null bytes
openssl_pkcs7_decrypt(): Argument #4 ($private_key) must not contain any null bytes
openssl_pkcs7_sign(): Argument #1 ($input_filename) must not contain any null bytes
openssl_pkcs7_sign(): Argument #2 ($output_filename) must not contain any null bytes

Warning: openssl_pkcs7_sign(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_pkcs7_sign(): Argument #3 ($certificate) must not contain any null bytes
openssl_pkcs7_sign(): Argument #4 ($private_key) must not contain any null bytes
openssl_pkcs7_sign(): Argument #7 ($untrusted_certificates_filename) must not contain any null bytes
openssl_pkcs7_verify(): Argument #1 ($input_filename) must not contain any null bytes
openssl_pkcs7_verify(): Argument #3 ($signers_certificates_filename) must not contain any null bytes
openssl_pkcs7_verify(): Argument #4 ($ca_info) array item must not contain any null bytes
openssl_pkcs7_verify(): Argument #5 ($untrusted_certificates_filename) must not contain any null bytes
openssl_pkcs7_verify(): Argument #6 ($content) must not contain any null bytes
openssl_pkcs7_verify(): Argument #7 ($output_filename) must not contain any null bytes

Warning: openssl_pkcs12_export(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_pkcs12_export(): Argument #1 ($certificate) must not contain any null bytes
openssl_pkcs12_export(): Argument #3 ($private_key) must not contain any null bytes
openssl_pkcs12_export(): Argument #5 ($options) option extracerts array item must not contain any null bytes

Warning: openssl_pkcs12_export_to_file(): X.509 Certificate cannot be retrieved in %s on line %d
openssl_pkcs12_export_to_file(): Argument #1 ($certificate) must not contain any null bytes
openssl_pkcs12_export_to_file(): Argument #2 ($output_filename) must not contain any null bytes
openssl_pkcs12_export_to_file(): Argument #3 ($private_key) must not contain any null bytes
openssl_pkcs12_export_to_file(): Argument #5 ($options) option extracerts array item must not contain any null bytes
