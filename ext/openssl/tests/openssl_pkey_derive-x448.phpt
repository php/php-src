--TEST--
openssl_pkey_derive() X448
--EXTENSIONS--
openssl
--SKIPIF--
<?php if (!defined("OPENSSL_KEYTYPE_X448")) print "skip"; ?>
--FILE--
<?php

$priv = openssl_pkey_get_private("-----BEGIN PRIVATE KEY-----
MEYCAQAwBQYDK2VvBDoEOABW8gZe1+BFOHcZVBkfQzh3zmBiw6FXptYcDF7ojpt3
FpYcdiiwj9ETnLHzDXBXxucgdfg866TM
-----END PRIVATE KEY-----
");

$pub = openssl_pkey_get_details(openssl_pkey_get_private("-----BEGIN PRIVATE KEY-----
MEYCAQAwBQYDK2VvBDoEOKSnpre4xISbWGaOu7URKR1lmr/p4diTP7rN7R/+hTv3
l098WrtwPEBim4nNogxKVHs9z7lqm2LF
-----END PRIVATE KEY-----
"))["key"];

echo bin2hex(openssl_pkey_derive($pub, $priv));
echo "\n";
?>
--EXPECT--
f57eea2028d773685334f3117d87d7f3b97d99373ff532a3ab50fbab8002c2e55bb4b39c11fc835d59e83d3d00c649e8566c013e0062fe9b
