--TEST--
IDN
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test IDN functions (procedural only)
 */

echo idn_to_ascii("t\xC3\xA4st.de")."\n";
echo urlencode(idn_to_utf8('xn--tst-qla.de'))."\n";

?>
--EXPECTF--
Deprecated: idn_to_ascii(): INTL_IDNA_VARIANT_2003 is deprecated in %s on line %d
xn--tst-qla.de

Deprecated: idn_to_utf8(): INTL_IDNA_VARIANT_2003 is deprecated in %s on line %d
t%C3%A4st.de
