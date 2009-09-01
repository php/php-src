--TEST--
Notice triggered by invalid configuration options
--CREDITS--
Christian Wenz <wenz@php.net>
--SKIPIF--
<?php
  if (!extension_loaded('tidy')) die ('skip tidy not present');
?>
--FILE--
<?php
$buffer = '<html></html>';
$config = array('bogus' => 'willnotwork');

$tidy = new tidy();
var_dump($tidy->parseString($buffer, $config));
?>
--EXPECTF--
Notice: tidy::parseString(): Unknown Tidy Configuration Option 'bogus' in %s on line %d
bool(true)
