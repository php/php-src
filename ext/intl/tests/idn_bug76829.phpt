--TEST--
Bug #76829 Incorrect validation of domain on idn_to_utf8() function
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

$punycode = idn_to_ascii('абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаеж.рф', IDNA_DEFAULT, INTL_IDNA_VARIANT_UTS46);

$unicode = idn_to_utf8($punycode, IDNA_DEFAULT, INTL_IDNA_VARIANT_UTS46);

var_dump($unicode);

?>
--EXPECT--
string(294) "абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаежзи.абвгдаеж.рф"
