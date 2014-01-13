--TEST--
openssl_pkcs7_mem_decrypt() and openssl_pkcs7_mem_encrypt tests
--SKIPIF--
<?php if (!extension_loaded("openssl")) print "skip"; ?>
--FILE--
<?php
$sMessage = "hey there, this is top secret message which gets de-/encrypted by memory soon";
$sCertfile = dirname( __FILE__ ) . '/cert.crt';
$sKeyfile = dirname( __FILE__ ) . '/private.key';
$sKeydata = file_get_contents($sKeyfile);
$sCertdata = file_get_contents($sCertfile);
$aMultiCerts = array( $sCertdata, $sCertdata );
$aAssocHeaders = array('To' => 'test@test', 'Subject' => 'testing openssl_pkcs7_mem_encrypt()');
$aHeaders = array_values($aAssocHeaders);
$aEmpty = array();
$sEncryptedMessage = "";
$sWrong = "sWrong";
$sEmpty = "";

echo "# Starting up encrypt tests\n";

$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sCertdata, $aHeaders );

if( !$bEncryptionStatus ) {
    die("encryption failed.");
}
if( trim($sEncryptedMessage) == "" ) {
    die("encryption was successful but returned string is empty");
}
var_dump( $bEncryptionStatus );

$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sCertdata, $aAssocHeaders );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sCertdata, $aEmpty );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sCertdata, $sWrong );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sWrong, $sEncryptedMessage, $sCertdata, $aHeaders );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sEmpty, $sEncryptedMessage, $sCertdata, $aHeaders );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sWrong, $aHeaders );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sEmpty, $aHeaders );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $sCertdata, $sEmpty );
var_dump( $bEncryptionStatus );
$bEncryptionStatus = openssl_pkcs7_mem_encrypt( $sMessage, $sEncryptedMessage, $aMultiCerts, $aHeaders );
var_dump( $bEncryptionStatus );

echo "# Starting up decrypt tests\n";
$sDecryptedMessage = "";
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, $sCertdata, $sKeydata );
$sShouldBeSuccessful = $sDecryptedMessage;
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, $sCertdata, $sWrong );
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, $sWrong, $sKeydata );
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, null, $sKeydata );
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sWrong, $sDecryptedMessage, $sCertdata, $sKeydata ); 
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEmpty, $sDecryptedMessage, $sCertdata, $sKeydata);
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, $sEmpty, $sKeydata);
var_dump( $bDecryptionStatus );
$bDecryptionStatus = openssl_pkcs7_mem_decrypt( $sEncryptedMessage, $sDecryptedMessage, $sCertdata, $sEmpty);
var_dump( $bDecryptionStatus );

var_dump( $sShouldBeSuccessful === $sMessage );

?>
--EXPECTF--
# Starting up encrypt tests
bool(true)
bool(true)
bool(true)

Warning: openssl_pkcs7_mem_encrypt() expects parameter 4 to be array, string given in %s on line %d
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)

Warning: openssl_pkcs7_mem_encrypt() expects parameter 4 to be array, string given in %s on line %d
bool(false)
bool(true)
# Starting up decrypt tests
bool(true)

Warning: openssl_pkcs7_mem_decrypt(): unable to get private key in %s on line %d
bool(false)

Warning: openssl_pkcs7_mem_decrypt(): unable to coerce parameter 3 to x509 cert in %s on line %d
bool(false)

Warning: openssl_pkcs7_mem_decrypt(): unable to coerce parameter 3 to x509 cert in %s on line %d
bool(false)

Warning: openssl_pkcs7_mem_decrypt(): Given indata is not PKCS7! in %s on line %d
bool(false)
bool(false)

Warning: openssl_pkcs7_mem_decrypt(): unable to coerce parameter 3 to x509 cert in %s on line %d
bool(false)

Warning: openssl_pkcs7_mem_decrypt(): unable to get private key in %s on line %d
bool(false)
bool(true)
