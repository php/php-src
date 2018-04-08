--TEST--
IDN UTS #46 API error tests
--SKIPIF--
<?php
	if (!extension_loaded('intl'))
		die('skip');
	if (!defined('INTL_IDNA_VARIANT_UTS46'))
		die('skip no UTS #46 API');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
echo "=> PHP level errors", "\n";

echo "bad args:", "\n";
var_dump(idn_to_ascii("", 0, array()));
var_dump(idn_to_ascii("", 0, INTL_IDNA_VARIANT_UTS46, $foo, null));

echo "bad variant:", "\n";
var_dump(idn_to_ascii("", 0, INTL_IDNA_VARIANT_UTS46 + 10));

echo "empty domain:", "\n";
var_dump(idn_to_ascii("", 0, INTL_IDNA_VARIANT_UTS46));

echo "fourth arg for 2003 variant (only notice raised):", "\n";
var_dump(idn_to_ascii("foo.com", 0, INTL_IDNA_VARIANT_2003, $foo));

echo "with error, but no details arg:", "\n";
var_dump(idn_to_ascii("www.fußball.com-", 0, INTL_IDNA_VARIANT_UTS46));

echo "with error, with details arg:", "\n";
var_dump(idn_to_ascii("www.fußball.com-", IDNA_NONTRANSITIONAL_TO_ASCII,
	INTL_IDNA_VARIANT_UTS46, $foo));
var_dump($foo);

echo "with error, with details arg, contextj:", "\n";
var_dump(idn_to_ascii(
		html_entity_decode("www.a&#x200D;b.com", 0, "UTF-8"),
		IDNA_NONTRANSITIONAL_TO_ASCII | IDNA_CHECK_CONTEXTJ,
		INTL_IDNA_VARIANT_UTS46, $foo));
var_dump($foo);
var_dump($foo["errors"]==IDNA_ERROR_CONTEXTJ);
--EXPECTF--
=> PHP level errors
bad args:

Warning: idn_to_ascii() expects parameter 3 to be int, array given in %s on line %d

Warning: idn_to_ascii(): idn_to_ascii: bad arguments in %s on line %d
NULL

Warning: idn_to_ascii() expects at most 4 parameters, 5 given in %s on line %d

Warning: idn_to_ascii(): idn_to_ascii: bad arguments in %s on line %d
NULL
bad variant:

Warning: idn_to_ascii(): idn_to_ascii: invalid variant, must be one of {INTL_IDNA_VARIANT_2003, INTL_IDNA_VARIANT_UTS46} in %s on line %d
bool(false)
empty domain:

Warning: idn_to_ascii(): idn_to_ascii: empty domain name in %s on line %d
bool(false)
fourth arg for 2003 variant (only notice raised):

Deprecated: idn_to_ascii(): INTL_IDNA_VARIANT_2003 is deprecated in %s on line %d

Notice: idn_to_ascii(): 4 arguments were provided, but INTL_IDNA_VARIANT_2003 only takes 3 - extra argument ignored in %s on line %d
string(7) "foo.com"
with error, but no details arg:
bool(false)
with error, with details arg:
bool(false)
array(3) {
  ["result"]=>
  string(23) "www.xn--fuball-cta.com-"
  ["isTransitionalDifferent"]=>
  bool(true)
  ["errors"]=>
  int(16)
}
with error, with details arg, contextj:
bool(false)
array(3) {
  ["result"]=>
  string(18) "www.xn--ab-m1t.com"
  ["isTransitionalDifferent"]=>
  bool(true)
  ["errors"]=>
  int(4096)
}
bool(true)
