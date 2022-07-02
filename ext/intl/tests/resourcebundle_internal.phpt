--TEST--
Test ResourceBundle::__construct() with internal ICU bundles
--EXTENSIONS--
intl
--SKIPIF--
<?php if( !defined('INTL_ICU_DATA_VERSION')) print 'skip INTL_ICU_DATA_VERSION constant not defined'; ?>
<?php if( version_compare(INTL_ICU_DATA_VERSION, '4.4', '<')) print 'skip for ICU >= 4.4'; ?>
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
