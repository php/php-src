--TEST--
int openssl_x509_checkpurpose ( mixed $x509cert , int $purpose [, array $cainfo = array() [, string $untrustedfile ]] ) function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; 
if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
$cert = "file://" . dirname(__FILE__) . "/cert.crt";
$bert = "file://" . dirname(__FILE__) . "/bug41033.pem";
$sert = "file://" . dirname(__FILE__) . "/san-cert.pem";
$cpca = dirname(__FILE__) . "/san-ca.pem";
$utfl = dirname(__FILE__) . "/sni_server_domain1.pem";

/*  int openssl_x509_checkpurpose ( mixed $x509cert , int $purpose);   */
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_CLIENT));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_NS_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_SIGN));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_ENCRYPT));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_CRL_SIGN));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_ANY));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_CLIENT));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_NS_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_SIGN));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_ENCRYPT));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_CRL_SIGN));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_ANY));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_CLIENT));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_NS_SSL_SERVER));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_SIGN));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_ENCRYPT));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_CRL_SIGN));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_ANY));

/* int openssl_x509_checkpurpose ( mixed $x509cert , int $purpose [, array $cainfo = array() ] ); */
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_CLIENT, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_NS_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_CRL_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_ANY, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_CLIENT, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_NS_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_CRL_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_ANY, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_CLIENT, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_NS_SSL_SERVER, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_CRL_SIGN, array($cpca)));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_ANY, array($cpca)));

/* int openssl_x509_checkpurpose ( mixed $x509cert , int $purpose [, array $cainfo = array() [, string $untrustedfile ]] ); function */
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_CLIENT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_NS_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_CRL_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($cert, X509_PURPOSE_ANY, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_CLIENT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_NS_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_CRL_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($bert, X509_PURPOSE_ANY, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_CLIENT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_NS_SSL_SERVER, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_SMIME_ENCRYPT, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_CRL_SIGN, array($cpca), $utfl));
var_dump(openssl_x509_checkpurpose($sert, X509_PURPOSE_ANY, array($cpca), $utfl));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
int(-1)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
