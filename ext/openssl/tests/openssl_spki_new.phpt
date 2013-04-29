--TEST--
Testing openssl_spki_new()
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

/* loop to create and verify results */
foreach($algos as $key => $value){
 echo "Creating new SPKAC with ".$key."...";
 $spki = openssl_spki_new($pkey, "sample_challenge_string", $value);
 if ((!empty($spki)) && (preg_match('/SPKAC=[a-zA-Z0-9\/\+]/', $spki)))
  echo "ok!\n";
}

openssl_free_key($pkey);
?>
--EXPECT--
Creating private key...ok!

Creating new SPKAC with MD5...ok!
Creating new SPKAC with SHA1...ok!
Creating new SPKAC with SHA256...ok!
Creating new SPKAC with SHA512...ok!