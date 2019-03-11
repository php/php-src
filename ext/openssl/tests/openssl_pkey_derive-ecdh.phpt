--TEST--
openssl_pkey_derive() ECDH
--SKIPIF--
<?php if (!extension_loaded("openssl") || !defined("OPENSSL_KEYTYPE_EC")) print "skip"; ?>
--FILE--
<?php

$priv = openssl_pkey_get_private("-----BEGIN EC PRIVATE KEY-----
MIHbAgEBBEEHI4mYyIOu8zQYAThKCgDIj4JAMekWFcrJSSa4K+C80xtrT07tz1Yj
mZdn+7/sAuZ4HBg56EzFBhLGxwGDakot1qAHBgUrgQQAI6GBiQOBhgAEARnoOOKF
c+1CNtrq2Jq0GvcBjIi1kJpQLfFF1RFgP/jVDeimSJi8elAtl6NqdikDVSIg6ZwT
6XOz6IdPRZsCMsWyAYeWz2jTRoT93nGLm9G96jwOm0VhLHjp3WtTY4kDp9dVHdhH
x3Nz35sz8u0CE6befv+Fxo5ORq373v9eDzp62Z8g
-----END EC PRIVATE KEY-----
");

$pub = openssl_pkey_get_public("-----BEGIN PUBLIC KEY-----
MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBGs5c8VCdd3VcOAUhuCzEB6uMUDob
lG5vtncWqvHfcnsR4uHEuufl24rbraVFyVeGr/BV0AfUnnhKGnaEtSDG9h4BMw5A
vHiBzBCZUlA1TUMSmNpedutkZul4h6gYNrzFtfjmbqSnC0732YgUIrr4yueOSL2E
N2IRPU2MF6S0S6i44MU=
-----END PUBLIC KEY-----
");

echo bin2hex(openssl_pkey_derive($pub,$priv));
echo "\n";
?>
--EXPECT--
01171967cc0ddc553b46c6a821502aaea44aa04e6933d897ea11222efa0556f2d5d972816676c9ccf4e2430a26e07193ad39373050f6e54e4059f17720d7dd667635
