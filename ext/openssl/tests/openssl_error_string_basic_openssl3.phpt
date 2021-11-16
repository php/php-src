--TEST--
openssl_error_string() tests (OpenSSL >= 3.0)
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (OPENSSL_VERSION_NUMBER < 0x30000000) die('skip For OpenSSL >= 3.0');
?>
--FILE--
<?php
// helper function to check openssl errors
function expect_openssl_errors($name, $expected_error_codes) {
    $expected_errors = array_fill_keys($expected_error_codes, false);
    $all_errors = array();
    while (($error_string = openssl_error_string()) !== false) {
    if (preg_match(",.+:([0-9A-F]+):.+,", $error_string, $m) > 0) {
            $error_code = $m[1];
            if (isset($expected_errors[$error_code])) {
                $expected_errors[$error_code] = true;
            }
        $all_errors[$error_code] = $error_string;
        } else {
        $all_errors[] = $error_string;
    }
    }

    $fail = false;
    foreach ($expected_errors as $error_code => $error_code_found) {
        if (!$error_code_found) {
            $fail = true;
            echo "$name: no error code $error_code\n";
        }
    }

    if (!$fail) {
        echo "$name: ok\n";
    } else {
    echo "$name: uncaught errors\n";
    foreach ($all_errors as $code => $str) {
        if (!isset($expected_errors[$code]) || !$expected_errors[$code]) {
            echo "\t", $code, ": ", $str, "\n";
        }
    }
    }
}

// helper for debugging errors
function dump_openssl_errors($name) {
    echo "\n$name\n";
    while (($error_string = openssl_error_string()) !== false) {
        var_dump($error_string);
    }
}

// common output file
$output_file =  __DIR__ . "/openssl_error_string_basic_output.tmp";
// invalid file for read is something that does not exist in current directory
$invalid_file_for_read = __DIR__ . "/invalid_file_for_read_operation.txt";
// invalid file for is the test dir as writing file to existing dir should always fail
$invalid_file_for_write = __DIR__;
// crt file
$crt_file = "file://" . __DIR__ . "/cert.crt";
// csr file
$csr_file = "file://" . __DIR__ . "/cert.csr";
// public key file
$public_key_file = "file://" .__DIR__ . "/public.key";
// private key file
$private_key_file = "file://" .__DIR__ . "/private_rsa_1024.key";
// private key file with password (password is 'php')
$private_key_file_with_pass = "file://" .__DIR__ . "/private_rsa_2048_pass_php.key";

// ENCRYPTION
$data = "test";
$method = "AES-128-ECB";
$enc_key = str_repeat('x', 40);
// error because password is longer then key length and
// EVP_CIPHER_CTX_set_key_length fails for AES
if (0) {
// TODO: This no longer errors!
openssl_encrypt($data, $method, $enc_key);
$enc_error = openssl_error_string();
var_dump($enc_error);
// make sure that error is cleared now
var_dump(openssl_error_string());
// internally OpenSSL ERR won't save more than 15 (16 - 1) errors so lets test it
for ($i = 0; $i < 20; $i++) {
    openssl_encrypt($data, $method, $enc_key);
}
$error_queue_size = 0;
while (($enc_error_new = openssl_error_string()) !== false) {
    if ($enc_error_new !== $enc_error) {
        echo "The new encoding error doesn't match the expected one\n";
    }
    ++$error_queue_size;
}
var_dump($error_queue_size);
echo "\n";
}

$err_pem_no_start_line = '0480006C';

// PKEY
$options = ['config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf'];
echo "PKEY errors\n";
// file for pkey (file:///) fails when opennig (BIO_new_file)
@openssl_pkey_export_to_file("file://" . $invalid_file_for_read, $output_file, null, $options);
expect_openssl_errors('openssl_pkey_export_to_file opening', ['10000080']);
// file or private pkey is not correct PEM - failing PEM_read_bio_PrivateKey
@openssl_pkey_export_to_file($csr_file, $output_file, null, $options);
expect_openssl_errors('openssl_pkey_export_to_file pem', ['1E08010C']);
// file to export cannot be written
@openssl_pkey_export_to_file($private_key_file, $invalid_file_for_write, null, $options);
expect_openssl_errors('openssl_pkey_export_to_file write', ['10080002']);
// successful export
@openssl_pkey_export($private_key_file_with_pass, $out, 'wrong pwd', $options);
expect_openssl_errors('openssl_pkey_export', ['1C800064', '04800065']);
// invalid x509 for getting public key
@openssl_pkey_get_public($private_key_file);
expect_openssl_errors('openssl_pkey_get_public', [$err_pem_no_start_line]);
// private encrypt with unknown padding
@openssl_private_encrypt("data", $crypted, $private_key_file, 1000);
expect_openssl_errors('openssl_private_encrypt', ['1C8000A5']);
// private decrypt with failed padding check
@openssl_private_decrypt("data", $crypted, $private_key_file);
expect_openssl_errors('openssl_private_decrypt', ['0200009F', '02000072']);
// public encrypt and decrypt with failed padding check and padding
@openssl_public_encrypt("data", $crypted, $public_key_file, 1000);
@openssl_public_decrypt("data", $crypted, $public_key_file);
expect_openssl_errors('openssl_private_(en|de)crypt padding', [$err_pem_no_start_line, '02000076', '0200008A', '02000072', '1C880004']);

// X509
echo "X509 errors\n";
// file for x509 (file:///) fails when opennig (BIO_new_file)
@openssl_x509_export_to_file("file://" . $invalid_file_for_read, $output_file);
expect_openssl_errors('openssl_x509_export_to_file open', ['10000080']);
// file or str cert is not correct PEM - failing PEM_read_bio_X509 or PEM_ASN1_read_bio
@openssl_x509_export_to_file($csr_file, $output_file);
expect_openssl_errors('openssl_x509_export_to_file pem', [$err_pem_no_start_line]);
// file to export cannot be written
@openssl_x509_export_to_file($crt_file, $invalid_file_for_write);
expect_openssl_errors('openssl_x509_export_to_file write', ['10080002']);
// checking purpose fails because there is no such purpose 1000
@openssl_x509_checkpurpose($crt_file, 1000);
expect_openssl_errors('openssl_x509_checkpurpose purpose', ['05800079']);

// CSR
echo "CSR errors\n";
// file for csr (file:///) fails when opennig (BIO_new_file)
@openssl_csr_get_subject("file://" . $invalid_file_for_read);
expect_openssl_errors('openssl_csr_get_subject open', ['10000080']);
// file or str csr is not correct PEM - failing PEM_read_bio_X509_REQ
@openssl_csr_get_subject($crt_file);
expect_openssl_errors('openssl_csr_get_subjec pem', [$err_pem_no_start_line]);

// other possible causes that are difficult to catch:
// - ASN1_STRING_to_UTF8 fails in add_assoc_name_entry
// - invalid php_x509_request field (NULL) would cause error with CONF_get_string

?>
--CLEAN--
<?php
$output_file =  __DIR__ . "/openssl_error_string_basic_output.tmp";
if (is_file($output_file)) {
    unlink($output_file);
}
?>
--EXPECT--
PKEY errors
openssl_pkey_export_to_file opening: ok
openssl_pkey_export_to_file pem: ok
openssl_pkey_export_to_file write: ok
openssl_pkey_export: ok
openssl_pkey_get_public: ok
openssl_private_encrypt: ok
openssl_private_decrypt: ok
openssl_private_(en|de)crypt padding: ok
X509 errors
openssl_x509_export_to_file open: ok
openssl_x509_export_to_file pem: ok
openssl_x509_export_to_file write: ok
openssl_x509_checkpurpose purpose: ok
CSR errors
openssl_csr_get_subject open: ok
openssl_csr_get_subjec pem: ok
