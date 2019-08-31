--TEST--
DOMDocument::validate() should validate an external DTD declaration
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
// reusing existing xml: http://cvs.php.net/viewvc.cgi/php-src/ext/dom/tests/dom.xml?view=co&content-type=text%2Fplain
// reusing existing dtd: http://cvs.php.net/viewvc.cgi/php-src/ext/dom/tests/dom.ent?view=co&content-type=text%2Fplain
$dom = new DOMDocument('1.0');
$dom->substituteEntities = true;
$dom->load(__DIR__.'/dom.xml');
var_dump($dom->validate());
?>
--EXPECT--
bool(true)
