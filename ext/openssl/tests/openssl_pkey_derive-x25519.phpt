--TEST--
openssl_pkey_derive() X25519
--EXTENSIONS--
openssl
--SKIPIF--
<?php if (!defined("OPENSSL_KEYTYPE_X25519")) print "skip"; ?>
--FILE--
<?php

$priv = openssl_pkey_get_private("-----BEGIN PRIVATE KEY-----
MC4CAQAwBQYDK2VuBCIEIEhxWrgdpbkAlbNgjSjuqodVy4w/SJzmxlsOlUdXUm5r
-----END PRIVATE KEY-----
");

$pub = openssl_pkey_get_public("-----BEGIN PUBLIC KEY-----
MCowBQYDK2VuAyEAaKrFGJv6L6JrVmkxhtU5+xrPZU2MHNQJcsGZY71WhX8=
-----END PUBLIC KEY-----
");

echo bin2hex(openssl_pkey_derive($pub, $priv));
echo "\n";
?>
--EXPECT--
76feeefed1d6bc01f3406d87759db371893ec6dc2cb912a130b33c3c91abcb21
