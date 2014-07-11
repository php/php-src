--TEST--
Bug #61860: Offsets may be wrong for grapheme_stri* functions
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$haystack = 'Auf der Straße nach Paris habe ich mit dem Fahrer gesprochen';
var_dump(
    grapheme_stripos($haystack, 'pariS '),
    grapheme_stristr($haystack, 'paRis '),
    grapheme_substr($haystack, grapheme_stripos($haystack, 'Paris'))
);

?>
--EXPECT--
int(20)
string(40) "Paris habe ich mit dem Fahrer gesprochen"
string(40) "Paris habe ich mit dem Fahrer gesprochen"
