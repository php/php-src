--TEST--
Bug #72506 (idn_to_ascii with INTL_IDNA_VARIANT_UTS46 fatals for long domain names)
--DESCRIPTION--
Actually, the $info array should be populated for these cases, but at least it's
not clear yet with which values exactly.
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip this test requires ext/intl');
if (!defined('INTL_IDNA_VARIANT_UTS46')) die('skip no UTS #46 API');
?>
--FILE--
<?php
// ASCII domain name with 255 characters
$domain = str_repeat('a.', 126) . 'aaa';
$result = idn_to_ascii($domain, 0, INTL_IDNA_VARIANT_UTS46, $info);
var_dump($result, $info);

// ASCII domain name with 256 characters – one character added
$domain .= 'a';
$result = idn_to_ascii($domain, 0, INTL_IDNA_VARIANT_UTS46, $info);
var_dump($result, $info);

// International domain name with cyrillic "ф" characters
$domain = str_repeat('ф.', 32) . 'a';
$result = idn_to_ascii($domain, 0, INTL_IDNA_VARIANT_UTS46, $info);
var_dump($result, $info);
?>
--EXPECT--
bool(false)
array(0) {
}
bool(false)
array(0) {
}
bool(false)
array(0) {
}
