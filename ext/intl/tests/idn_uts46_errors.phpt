--TEST--
IDN UTS #46 API error tests
--EXTENSIONS--
intl
--SKIPIF--
<?php
    if (!defined('INTL_IDNA_VARIANT_UTS46'))
        die('skip no UTS #46 API');
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
echo "=> PHP level errors", "\n";

echo "bad variant:", "\n";
try {
    var_dump(idn_to_ascii("domain", 0, INTL_IDNA_VARIANT_UTS46 + 10));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "empty domain:", "\n";
try {
    var_dump(idn_to_ascii("", 0, INTL_IDNA_VARIANT_UTS46));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "with error, but no details arg:", "\n";
var_dump(idn_to_ascii("www.fußball.com-", 0, INTL_IDNA_VARIANT_UTS46));

echo "with error, with details arg:", "\n";
var_dump(idn_to_ascii("www.fußball.com-", IDNA_NONTRANSITIONAL_TO_ASCII,
    INTL_IDNA_VARIANT_UTS46, $foo));
var_dump($foo);

echo "with error, with details arg, context:", "\n";
var_dump(idn_to_ascii(
        html_entity_decode("www.a&#x200D;b.com", 0, "UTF-8"),
        IDNA_NONTRANSITIONAL_TO_ASCII | IDNA_CHECK_CONTEXTJ,
        INTL_IDNA_VARIANT_UTS46, $foo));
var_dump($foo);
var_dump($foo["errors"]==IDNA_ERROR_CONTEXTJ);

?>
--EXPECT--
=> PHP level errors
bad variant:
ValueError: idn_to_ascii(): Argument #2 ($flags) must be INTL_IDNA_VARIANT_UTS46
empty domain:
ValueError: idn_to_ascii(): Argument #1 ($domain) must not be empty
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
with error, with details arg, context:
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
