--TEST--
Basic UConverter::transcode() usage
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
var_dump(UConverter::transcode("This is an ascii string", 'utf-8', 'latin1'));
// urlencode so that non-ascii shows up parsable in phpt file
var_dump(urlencode(UConverter::transcode("Espa\xF1ol", 'utf-8', 'latin1')));
var_dump(urlencode(UConverter::transcode("Stra\xDFa",  'utf-8', 'latin1')));

var_dump(bin2hex(UConverter::transcode("\xE4", 'utf-8', 'koi8-r')));
--EXPECT--
string(23) "This is an ascii string"
string(12) "Espa%C3%B1ol"
string(11) "Stra%C3%9Fa"
string(4) "d094"
