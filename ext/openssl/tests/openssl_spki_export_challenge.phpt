--TEST--
Testing openssl_spki_export_challenge()
<<<<<<< HEAD
Creates SPKAC for all available key sizes & signature algorithms and exports challenge
--INI--
error_reporting=0
=======
>>>>>>> PHP-5.5
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

<<<<<<< HEAD
/* array of private key sizes to test */
$ksize = array('1024'=>1024,
			   '2048'=>2048,
			   '4096'=>4096);

/* array of available hashings to test */
$algo = array('md4'=>OPENSSL_ALGO_MD4,
              'md5'=>OPENSSL_ALGO_MD5,
              'sha1'=>OPENSSL_ALGO_SHA1,
			  'sha224'=>OPENSSL_ALGO_SHA224,
              'sha256'=>OPENSSL_ALGO_SHA256,
              'sha384'=>OPENSSL_ALGO_SHA384,
              'sha512'=>OPENSSL_ALGO_SHA512,
              'rmd160'=>OPENSSL_ALGO_RMD160);

/* loop over key sizes for test */
foreach($ksize as $k => $v) {

	/* generate new private key of specified size to use for tests */
	$pkey = openssl_pkey_new(array('digest_alg' => 'sha512',
	                               'private_key_type' => OPENSSL_KEYTYPE_RSA,
	                               'private_key_bits' => $v));
	openssl_pkey_export($pkey, $pass);

	/* loop to create and verify results */
	foreach($algo as $key => $value) {
		$spkac = openssl_spki_new($pkey, _uuid(), $value);
		var_dump(openssl_spki_export_challenge(preg_replace('/SPKAC=/', '', $spkac)));
		var_dump(openssl_spki_export_challenge($spkac.'Make it fail'));
	}
	openssl_free_key($pkey);
}

/* generate a random challenge */
function _uuid()
{
 return sprintf('%04x%04x-%04x-%04x-%04x-%04x%04x%04x', mt_rand(0, 0xffff),
                mt_rand(0, 0xffff), mt_rand(0, 0xffff), mt_rand(0, 0x0fff) | 0x4000,
                mt_rand(0, 0x3fff) | 0x8000, mt_rand(0, 0xffff),
                mt_rand(0, 0xffff), mt_rand(0, 0xffff));
}

?>
--EXPECTREGEX--
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
string\(36\) \"[0-9a-f]{8}\-([0-9a-f]{4}\-){3}[0-9a-f]{12}\"
bool\(false\)
=======
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

/* loop to create, export challenge and verify */
foreach($algos as $key => $value){
 echo "Exporting challenge from SPKAC with ".$key."...";
 $spki = openssl_spki_new($pkey, "sample_challenge_string", $value);
 if ((!empty($spki)) && (preg_match('/SPKAC=[a-zA-Z0-9\/\+]/', $spki)))
  $r = openssl_spki_export_challenge(preg_replace('/SPKAC=/', '', $spki)); 
  if ($r === "sample_challenge_string")
   echo "ok!\n";
}

openssl_free_key($pkey);
?>
--EXPECT--
Creating private key...ok!

Exporting challenge from SPKAC with MD5...ok!
Exporting challenge from SPKAC with SHA1...ok!
Exporting challenge from SPKAC with SHA256...ok!
Exporting challenge from SPKAC with SHA512...ok!
>>>>>>> PHP-5.5
