--TEST--
openssl_csr_new() with array DN entry
--EXTENSIONS--
openssl
--FILE--
<?php

$a = array();

$conf = array('config' => __DIR__ . DIRECTORY_SEPARATOR . 'openssl.cnf');

// options type check
$x = openssl_pkey_new($conf);
$csr = openssl_csr_new(
    [    
        "countryName" => "GB",
        "stateOrProvinceName" => "Somerset",
        "localityName" => "Glastonbury",
        "organizationName" => ["o1" => "PHP", "o2" => "PHP Foundation"],
        "organizationalUnitName" => ["PHP Doc team", "PHP Admin team", "PHP Core team"],
        "commonName" => "test.php.net",
        "emailAddress" => "php.test@example.com"
    ],
    $x,
    $conf + ["x509_extensions" => 0xDEADBEEF]
);

var_dump(openssl_csr_get_subject($csr));

?>
--EXPECT--
array(7) {
  ["C"]=>
  string(2) "GB"
  ["ST"]=>
  string(8) "Somerset"
  ["L"]=>
  string(11) "Glastonbury"
  ["O"]=>
  array(2) {
    [0]=>
    string(3) "PHP"
    [1]=>
    string(14) "PHP Foundation"
  }
  ["OU"]=>
  array(3) {
    [0]=>
    string(12) "PHP Doc team"
    [1]=>
    string(14) "PHP Admin team"
    [2]=>
    string(13) "PHP Core team"
  }
  ["CN"]=>
  string(12) "test.php.net"
  ["emailAddress"]=>
  string(20) "php.test@example.com"
}