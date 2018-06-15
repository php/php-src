--TEST--
Test ResourceBundle::__construct() with internal ICU bundles
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) || !defined('INTL_ICU_DATA_VERSION') || version_compare(INTL_ICU_DATA_VERSION, '4.4', '<') ) print 'skip'; ?>
--FILE--
<?php
$b = new ResourceBundle('de_DE', 'ICUDATA-region');
var_dump($b->get('Countries')->get('DE'));

$b = new ResourceBundle('icuver', 'ICUDATA');
var_dump($b->get('ICUVersion') !== NULL);

$b = new ResourceBundle('supplementalData', 'ICUDATA', false);
var_dump($b->get('cldrVersion') !== NULL);
?>
--EXPECT--
string(11) "Deutschland"
bool(true)
bool(true)
