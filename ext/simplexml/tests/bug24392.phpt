--TEST--
Bug #24392: empty namespaces causing confusion
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
$s = simplexml_load_file('ext/simplexml/tests/bug24392.xml');
foreach ($s->item as $item) {
	echo $item->title . "\n";
}
?>
--EXPECT--
EU Parliament to Vote on New Patent Rules
Most Powerful Amateur Rocket in Canada
GF FX 5900 Ultra vs. ATi Radeon 9800 Pro
PHP 5 Beta 1
Engaging with the OSS Community
Pure Math, Pure Joy
Windows Tech Writer Looks at Linux
US Cell Phone Users Discover SMS Spam
Verizon Sues Nextel For Espionage
Introduction to Debian
