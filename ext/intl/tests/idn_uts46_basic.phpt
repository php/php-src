--TEST--
IDN UTS #46 API basic tests
--SKIPIF--
<?php
	if (!extension_loaded('intl'))
		die('skip');
	if (!defined('INTL_IDNA_VARIANT_UTS46'))
		die('skip no UTS #46 API');
--FILE--
<?php
$utf8dn = "www.fußball.com";
$asciiNonTrans = "www.xn--fuball-cta.com";

echo "all ok, no details:", "\n";
var_dump(idn_to_ascii($utf8dn,
	IDNA_NONTRANSITIONAL_TO_ASCII, INTL_IDNA_VARIANT_UTS46));

echo "all ok, no details, transitional:", "\n";
var_dump(idn_to_ascii($utf8dn, 0, INTL_IDNA_VARIANT_UTS46));

echo "all ok, with details:", "\n";
var_dump(idn_to_ascii($utf8dn, IDNA_NONTRANSITIONAL_TO_ASCII,
	INTL_IDNA_VARIANT_UTS46, $info));
var_dump($info);

echo "reverse, ok, with details:", "\n";
var_dump(idn_to_utf8($asciiNonTrans, 0, INTL_IDNA_VARIANT_UTS46, $info));
var_dump($info);
--EXPECT--
all ok, no details:
string(22) "www.xn--fuball-cta.com"
all ok, no details, transitional:
string(16) "www.fussball.com"
all ok, with details:
string(22) "www.xn--fuball-cta.com"
array(3) {
  ["result"]=>
  string(22) "www.xn--fuball-cta.com"
  ["isTransitionalDifferent"]=>
  bool(true)
  ["errors"]=>
  int(0)
}
reverse, ok, with details:
string(16) "www.fußball.com"
array(3) {
  ["result"]=>
  string(16) "www.fußball.com"
  ["isTransitionalDifferent"]=>
  bool(false)
  ["errors"]=>
  int(0)
}
