--TEST--
#41033, enable signing with DSA keys
--EXTENSIONS--
openssl
--SKIPIF--
<?php
if (!defined("OPENSSL_KEYTYPE_DSA")) die("skip DSA disabled");
?>
--FILE--
<?php
$prv = 'file://' . __DIR__ . '/' . 'bug41033.pem';
$pub = 'file://' . __DIR__ . '/' . 'bug41033pub.pem';


$prkeyid = openssl_get_privatekey($prv, "1234");
$ct = "Hello I am some text!";
openssl_sign($ct, $signature, $prkeyid, OPENSSL_ALGO_SHA256);
echo "Signature: ".base64_encode($signature) . "\n";

$pukeyid = openssl_get_publickey($pub);
$valid = openssl_verify($ct, $signature, $pukeyid, OPENSSL_ALGO_SHA256);
echo "Signature validity: " . $valid . "\n";


?>
--EXPECTF--
Signature: %s
Signature validity: 1
