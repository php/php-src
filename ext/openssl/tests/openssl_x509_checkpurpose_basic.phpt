--TEST--
int openssl_x509_checkpurpose ( mixed $x509cert , int $purpose [, array $cainfo = array() [, string $untrustedfile ]] ) function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--EXTENSIONS--
openssl
--SKIPIF--
<?php if (OPENSSL_VERSION_NUMBER < 0x10000000) die("skip Output requires OpenSSL 1.0");
?>
--FILE--
<?php
include 'CertificateGenerator.inc';
$certificateGenerator = new CertificateGenerator();
$certificateGenerator->saveCaCert(__DIR__ . "/san-cert.pem");

$cert = "file://" . __DIR__ . "/cert.crt";
$bert = "file://" . __DIR__ . "/bug41033.pem";
$sert = "file://" . __DIR__ . "/san-cert.pem";
$cpca = __DIR__ . "/san-cert.pem";
$utfl = __DIR__ . "/sni_server_uk.pem";
$rcrt = openssl_x509_read($cert);

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
var_dump(openssl_x509_checkpurpose($rcrt, X509_PURPOSE_ANY, array($cpca)));
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
--CLEAN--
<?php
@unlink(__DIR__ . "/san-cert.pem");
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
