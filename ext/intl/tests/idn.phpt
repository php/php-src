--TEST--
IDN
--INI--
unicode.runtime_encoding="utf-8"
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Test IDN functions (procedural only)
 */

echo idn_to_ascii(b"t\xC3\xA4st.de")."\n";
echo urlencode((binary)idn_to_unicode('xn--tst-qla.de'))."\n";
echo urlencode((binary)idn_to_utf8('xn--tst-qla.de'))."\n";

?>
--EXPECT--
xn--tst-qla.de
t%C3%A4st.de
t%C3%A4st.de