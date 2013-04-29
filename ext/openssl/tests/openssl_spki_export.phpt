--TEST--
Testing openssl_spki_export()
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

/* private key creation */
echo "Creating private key...";
$pkey = openssl_pkey_new();
echo ($pkey !== false) ?
 "ok!\n\n" : "error creating private key\n\n";

/* testing all signing algorithms */
$algos = array('MD5'=>OPENSSL_ALGO_MD5,
               'SHA1'=>OPENSSL_ALGO_SHA1,
			   'SHA256'=>OPENSSL_ALGO_SHA256,
			   'SHA512'=>OPENSSL_ALGO_SHA512);

/* loop to create, export public keys and verify */
foreach($algos as $key => $value){
 echo "Exporting public key from SPKAC with ".$key."...";
 $spki = openssl_spki_new($pkey, "sample_challenge_string", $value);
 if ((!empty($spki)) && (preg_match('/SPKAC=[a-zA-Z0-9\/\+]/', $spki)))
  $r = openssl_spki_export(preg_replace('/SPKAC=/', '', $spki)); 
  if (preg_match("/-----BEGIN PUBLIC KEY-----/", $r))
   echo "ok!\n";
}

openssl_free_key($pkey);
?>
--EXPECT--
Creating private key...ok!

Exporting public key from SPKAC with MD5...ok!
Exporting public key from SPKAC with SHA1...ok!
Exporting public key from SPKAC with SHA256...ok!
Exporting public key from SPKAC with SHA512...ok!