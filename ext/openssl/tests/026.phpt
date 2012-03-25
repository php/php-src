--TEST--
openssl_spki_new(), openssl_spki_verify(), openssl_spki_export(), openssl_spki_export_challenge(), openssl_spki_details()
--SKIPIF--
<?php
if (!extension_loaded("openssl")) die("skip");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

echo "Creating private key\n";
$key = openssl_pkey_new();
if ($key === false)
 die("failed to create private key\n");

echo "Creating new SPKAC with defaults\n";
if (!function_exists("openssl_spki_new"))
 die("openssl_spki_new() does not exist\n");

$spki = openssl_spki_new($key, "sample_challenge_string");
if ($spki === false)
 die("could not create spkac\n");

echo "Verifying SPKAC using defaults\n";
if (!function_exists("openssl_spki_verify"))
 die("openssl_spki_verify() does not exist\n");

$a = openssl_spki_verify(preg_replace("/SPKAC=/", "", $spki));
if ($a === false)
 die("could not verify spkac\n");

echo "Exporting challenge using defaults\n";
if (!function_exists("openssl_spki_export_challenge"))
 die("openssl_spki_export_challenge() does not exist\n");

$b = openssl_spki_export_challenge(preg_replace("/SPKAC=/", "", $spki));
if ($b !== "sample_challenge_string")
 die("could not verify challenge string from spkac\n");

echo "Exporting public key from SPKAC using defaults\n";
if (!function_exists("openssl_spki_export"))
 die("openssl_spki_export() does not exist\n");

$c = openssl_spki_export(preg_replace("/SPKAC=/", '', $spki));
if ($c === "")
 die("could not export public key from spkac\n");

echo "Generating details of SPKAC structure using defaults\n";
if (!function_exists("openssl_spki_details"))
 die("openssl_spki_details() does not exist\n");

$d = openssl_spki_details(preg_replace('/SPKAC=/', '', $spki));
if ($d === "")
 die("could not obtain details from spkac\n");

unset($spki, $a, $b, $c, $d);

echo "Creating new SPKAC using md5 signature\n";
if (!function_exists("openssl_spki_new"))
 die("openssl_spki_new() does not exist\n");

$spki = openssl_spki_new($key, "sample_challenge_string", "md5");
if ($spki === false)
 die("could not create spkac\n");

echo "Verifying SPKAC using md5 signature\n";
if (!function_exists("openssl_spki_verify"))
 die("openssl_spki_verify() does not exist\n");

$a = openssl_spki_verify(preg_replace("/SPKAC=/", "", $spki));
if ($a === false)
 die("could not verify spkac\n");

echo "Exporting challenge using md5 signature\n";
if (!function_exists("openssl_spki_export_challenge"))
 die("openssl_spki_export_challenge() does not exist\n");

$b = openssl_spki_export_challenge(preg_replace("/SPKAC=/", "", $spki));
if ($b !== "sample_challenge_string")
 die("could not verify challenge string from spkac\n");

echo "Exporting public key from SPKAC using md5 signature\n";
if (!function_exists("openssl_spki_export"))
 die("openssl_spki_export() does not exist\n");

$c = openssl_spki_export(preg_replace("/SPKAC=/", '', $spki));
if ($c === "")
 die("could not export public key from spkac\n");

echo "Generating details of SPKAC structure using md5 signature\n";
if (!function_exists("openssl_spki_details"))
 die("openssl_spki_details() does not exist\n");

$d = openssl_spki_details(preg_replace('/SPKAC=/', '', $spki));
if ($d === "")
 die("could not obtain details from spkac\n");

unset($spki, $a, $b, $c, $d);

echo "Creating new SPKAC using sha1 signature\n";
if (!function_exists("openssl_spki_new"))
 die("openssl_spki_new() does not exist\n");

$spki = openssl_spki_new($key, "sample_challenge_string", "sha1");
if ($spki === false)
 die("could not create spkac\n");

echo "Verifying SPKAC using sha1 signature\n";
if (!function_exists("openssl_spki_verify"))
 die("openssl_spki_verify() does not exist\n");

$a = openssl_spki_verify(preg_replace("/SPKAC=/", "", $spki));
if ($a === false)
 die("could not verify spkac\n");

echo "Exporting challenge using sha1 signature\n";
if (!function_exists("openssl_spki_export_challenge"))
 die("openssl_spki_export_challenge() does not exist\n");

$b = openssl_spki_export_challenge(preg_replace("/SPKAC=/", "", $spki));
if ($b !== "sample_challenge_string")
 die("could not verify challenge string from spkac\n");

echo "Exporting public key from SPKAC using sha1 signature\n";
if (!function_exists("openssl_spki_export"))
 die("openssl_spki_export() does not exist\n");

$c = openssl_spki_export(preg_replace("/SPKAC=/", '', $spki));
if ($c === "")
 die("could not export public key from spkac\n");

echo "Generating details of SPKAC structure using sha1 signature\n";
if (!function_exists("openssl_spki_details"))
 die("openssl_spki_details() does not exist\n");

$d = openssl_spki_details(preg_replace('/SPKAC=/', '', $spki));
if ($d === "")
 die("could not obtain details from spkac\n");

unset($spki, $a, $b, $c, $d);

echo "Creating new SPKAC using sha512 signature\n";
if (!function_exists("openssl_spki_new"))
 die("openssl_spki_new() does not exist\n");

$spki = openssl_spki_new($key, "sample_challenge_string", "sha512");
if ($spki === false)
 die("could not create spkac\n");

echo "Verifying SPKAC using sha512 signature\n";
if (!function_exists("openssl_spki_verify"))
 die("openssl_spki_verify() does not exist\n");

$a = openssl_spki_verify(preg_replace("/SPKAC=/", "", $spki));
if ($a === false)
 die("could not verify spkac\n");

echo "Exporting challenge using sha512 signature\n";
if (!function_exists("openssl_spki_export_challenge"))
 die("openssl_spki_export_challenge() does not exist\n");

$b = openssl_spki_export_challenge(preg_replace("/SPKAC=/", "", $spki));
if ($b !== "sample_challenge_string")
 die("could not verify challenge string from spkac\n");

echo "Exporting public key from SPKAC using sha512 signature\n";
if (!function_exists("openssl_spki_export"))
 die("openssl_spki_export() does not exist\n");

$c = openssl_spki_export(preg_replace("/SPKAC=/", '', $spki));
if ($c === "")
 die("could not export public key from spkac\n");

echo "Generating details of SPKAC structure using sha512 signature\n";
if (!function_exists("openssl_spki_details"))
 die("openssl_spki_details() does not exist\n");

$d = openssl_spki_details(preg_replace('/SPKAC=/', '', $spki));
if ($d === "")
 die("could not obtain details from spkac\n");

echo "OK!\n";

openssl_free_key($key);
?>
--EXPECT--
Creating private key
Creating new SPKAC with defaults
Verifying SPKAC using defaults
Exporting challenge using defaults
Exporting public key from SPKAC using defaults
Generating details of SPKAC structure using defaults
Creating new SPKAC using md5 signature
Verifying SPKAC using md5 signature
Exporting challenge using md5 signature
Exporting public key from SPKAC using md5 signature
Generating details of SPKAC structure using md5 signature
Creating new SPKAC using sha1 signature
Verifying SPKAC using sha1 signature
Exporting challenge using sha1 signature
Exporting public key from SPKAC using sha1 signature
Generating details of SPKAC structure using sha1 signature
Creating new SPKAC using sha512 signature
Verifying SPKAC using sha512 signature
Exporting challenge using sha512 signature
Exporting public key from SPKAC using sha512 signature
Generating details of SPKAC structure using sha512 signature
OK!
