--TEST--
openssl_csr_export_to_file() tests
--CREDITS--
 Mark Railton
 mark@markrailton.com
 PHP TestFest 2017 - PHPDublin
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$dn = array(
    "countryName" => "IE",
    "stateOrProvinceName" => "Leinster",
    "localityName" => "Dublin",
    "organizationName" => "PHPDublin",
    "organizationalUnitName" => "PHP Documentation Team",
    "commonName" => "Mark Railton",
    "emailAddress" => "mark@markrailton.com"
);

$privkey = openssl_pkey_new(array(
    "private_key_bits" => 2048,
    "private_key_type" => OPENSSL_KEYTYPE_RSA,
));

// Generate the CSR
$csr = openssl_csr_new($dn, $privkey, array('digest_alg' => 'sha256'));

// Export CSR to file
$out = openssl_csr_export_to_file($csr, 'example-csr.pem');

// Did it export?
var_dump($out);

// Read the file
$file = file_get_contents('example-csr.pem', FILE_USE_INCLUDE_PATH);

// Check that the CSR is valid
$valid = openssl_csr_get_subject($file);

// Start outputting the various parts of the subject to confirm the exported file was re-read properly
var_dump($valid['C']);
var_dump($valid['ST']);
var_dump($valid['L']);
var_dump($valid['O']);
var_dump($valid['OU']);
var_dump($valid['CN']);
var_dump($valid['emailAddress']);
?>
--EXPECTF--
bool(true)
string(2) "IE"
string(8) "Leinster"
string(6) "Dublin"
string(9) "PHPDublin"
string(22) "PHP Documentation Team"
string(12) "Mark Railton"
string(20) "mark@markrailton.com"