--TEST--
array openssl_get_md_methods ([ bool $aliases = false ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) print "skip";
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$getMdMethodsTrue = openssl_get_md_methods(true);
$getMdMethodsFalse = openssl_get_md_methods(false);

if ((is_array($getMdMethodsTrue)) && (!is_null($getMdMethodsTrue))) {
 print("okey");
} else {
 print("openssl has failure to get cipher method with true argument");
}

if ((is_array($getMdMethodsFalse)) && (!is_null($getMdMethodsFalse))) {
 print("okey");
} else {
 print("openssl has failure to get cipher method with true argument");
}
?>
--EXPECT--
okey
okey
array(53) {
  [0]=>
  string(3) "DSA"
  [1]=>
  string(7) "DSA-SHA"
  [2]=>
  string(8) "DSA-SHA1"
  [3]=>
  string(12) "DSA-SHA1-old"
  [4]=>
  string(4) "DSS1"
  [5]=>
  string(3) "MD4"
  [6]=>
  string(3) "MD5"
  [7]=>
  string(9) "RIPEMD160"
  [8]=>
  string(7) "RSA-MD4"
  [9]=>
  string(7) "RSA-MD5"
  [10]=>
  string(13) "RSA-RIPEMD160"
  [11]=>
  string(7) "RSA-SHA"
  [12]=>
  string(8) "RSA-SHA1"
  [13]=>
  string(10) "RSA-SHA1-2"
  [14]=>
  string(10) "RSA-SHA224"
  [15]=>
  string(10) "RSA-SHA256"
  [16]=>
  string(10) "RSA-SHA384"
  [17]=>
  string(10) "RSA-SHA512"
  [18]=>
  string(3) "SHA"
  [19]=>
  string(4) "SHA1"
  [20]=>
  string(6) "SHA224"
  [21]=>
  string(6) "SHA256"
  [22]=>
  string(6) "SHA384"
  [23]=>
  string(6) "SHA512"
  [24]=>
  string(13) "dsaEncryption"
  [25]=>
  string(10) "dsaWithSHA"
  [26]=>
  string(11) "dsaWithSHA1"
  [27]=>
  string(4) "dss1"
  [28]=>
  string(15) "ecdsa-with-SHA1"
  [29]=>
  string(3) "md4"
  [30]=>
  string(20) "md4WithRSAEncryption"
  [31]=>
  string(3) "md5"
  [32]=>
  string(20) "md5WithRSAEncryption"
  [33]=>
  string(6) "ripemd"
  [34]=>
  string(9) "ripemd160"
  [35]=>
  string(16) "ripemd160WithRSA"
  [36]=>
  string(6) "rmd160"
  [37]=>
  string(3) "sha"
  [38]=>
  string(4) "sha1"
  [39]=>
  string(21) "sha1WithRSAEncryption"
  [40]=>
  string(6) "sha224"
  [41]=>
  string(23) "sha224WithRSAEncryption"
  [42]=>
  string(6) "sha256"
  [43]=>
  string(23) "sha256WithRSAEncryption"
  [44]=>
  string(6) "sha384"
  [45]=>
  string(23) "sha384WithRSAEncryption"
  [46]=>
  string(6) "sha512"
  [47]=>
  string(23) "sha512WithRSAEncryption"
  [48]=>
  string(20) "shaWithRSAEncryption"
  [49]=>
  string(8) "ssl2-md5"
  [50]=>
  string(8) "ssl3-md5"
  [51]=>
  string(9) "ssl3-sha1"
  [52]=>
  string(9) "whirlpool"
}
array(24) {
  [0]=>
  string(3) "DSA"
  [1]=>
  string(7) "DSA-SHA"
  [2]=>
  string(3) "MD4"
  [3]=>
  string(3) "MD5"
  [4]=>
  string(9) "RIPEMD160"
  [5]=>
  string(3) "SHA"
  [6]=>
  string(4) "SHA1"
  [7]=>
  string(6) "SHA224"
  [8]=>
  string(6) "SHA256"
  [9]=>
  string(6) "SHA384"
  [10]=>
  string(6) "SHA512"
  [11]=>
  string(13) "dsaEncryption"
  [12]=>
  string(10) "dsaWithSHA"
  [13]=>
  string(15) "ecdsa-with-SHA1"
  [14]=>
  string(3) "md4"
  [15]=>
  string(3) "md5"
  [16]=>
  string(9) "ripemd160"
  [17]=>
  string(3) "sha"
  [18]=>
  string(4) "sha1"
  [19]=>
  string(6) "sha224"
  [20]=>
  string(6) "sha256"
  [21]=>
  string(6) "sha384"
  [22]=>
  string(6) "sha512"
  [23]=>
  string(9) "whirlpool"
}
