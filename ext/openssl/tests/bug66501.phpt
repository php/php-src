--TEST--
Bug #66501: EC private key support in openssl_sign
--SKIPIF--
<?php 
if (!extension_loaded("openssl")) die("skip");
if (!defined('OPENSSL_KEYTYPE_EC')) die("skip no EC available");
--FILE--
<?php
$pkey = 'ASN1 OID: prime256v1
-----BEGIN EC PARAMETERS-----
BggqhkjOPQMBBw==
-----END EC PARAMETERS-----
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEILPkqoeyM7XgwYkuSj3077lrsrfWJK5LqMolv+m2oOjZoAoGCCqGSM49
AwEHoUQDQgAEPq4hbIWHvB51rdWr8ejrjWo4qVNWVugYFtPg/xLQw0mHkIPZ4DvK
sqOTOnMoezkbSmVVMuwz9flvnqHGmQvmug==
-----END EC PRIVATE KEY-----';
$key = openssl_pkey_get_private($pkey);
$res = openssl_sign($data ='alpha', $sign, $key, 'SHA1');
var_dump($res);
--EXPECTF--
bool(true)
