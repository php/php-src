--TEST--
Test ResourceBundle::__construct() with internal ICU bundles
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$b = new ResourceBundle('de_DE', 'ICUDATA-region');
var_dump($b->get('Countries')->get('DE'));

$b = new ResourceBundle('icuver', 'ICUDATA');
var_dump($b->get('ICUVersion') !== NULL);

$b = new ResourceBundle('supplementalData', 'ICUDATA', false);
var_dump($b->get('cldrVersion') !== NULL);
?>
--EXPECTF--
string(11) "Deutschland"
bool(true)
bool(true)
