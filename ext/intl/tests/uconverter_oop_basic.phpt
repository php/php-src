--TEST--
Basic UConverter::convert() usage
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$c = new UConverter('utf-8', 'latin1');
var_dump($c->convert("This is an ascii string"));
// urlencode so that non-ascii shows up parsable in phpt file
var_dump(urlencode($c->convert("Espa\xF1ol"))); // U+00F1 LATIN SMALL LETTER N WITH TILDE
var_dump(urlencode($c->convert("Stra\xDFa")));  // U+00DF LATIN SMALL LETTER SHARP S
var_dump(urlencode($c->convert("Stra\xC3\x9Fa", true))); // Reverse prior op

$k = new UConverter('utf-8', 'koi8-r');
var_dump(bin2hex($k->convert("\xE4"))); // U+0414 CYRILLIC CAPITAL LETTER DE
--EXPECT--
string(23) "This is an ascii string"
string(12) "Espa%C3%B1ol"
string(11) "Stra%C3%9Fa"
string(8) "Stra%DFa"
string(4) "d094"
