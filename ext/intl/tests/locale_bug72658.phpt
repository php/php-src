--TEST--
Bug #72658 Locale::lookup() / locale_lookup() hangs if no match found
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

var_dump(
    Locale::lookup(['en-Latn-US', 'sl', 'sl-IT'], 'en-US', true, 'de-DE'),
    Locale::lookup(['en-Latn-US', 'sl', 'sl-IT'], 'en-US', false, 'de-DE')
);

?>
--EXPECT--
string(5) "de-DE"
string(5) "de-DE"
