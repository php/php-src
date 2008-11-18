--TEST--
#41033, enable signing with DSA keys
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip, openssl required");
if (OPENSSL_VERSION_NUMBER < 0x009070af) die("skip");
?>
--FILE--
<?php
$prv = 'file://' . dirname(__FILE__) . '/' . 'bug41033.pem';
$pub = 'file://' . dirname(__FILE__) . '/' . 'bug41033pub.pem';


$prkeyid = openssl_get_privatekey($prv, "1234");
$ct = "Hello I am some text!";
openssl_sign($ct, $signature, $prkeyid, OPENSSL_ALGO_DSS1);
echo "Signature: ".base64_encode($signature) . "\n";

$pukeyid = openssl_get_publickey($pub);
$valid = openssl_verify($ct, $signature, $pukeyid, OPENSSL_ALGO_DSS1);
echo "Signature validity: " . $valid . "\n";


?>
--EXPECTF--
Signature: %s
Signature validity: 1
