--TEST--
libxml_set_external_entity_loader() variation: restore original handler; returning NULL
--SKIPIF--
<?php if (!extension_loaded('dom')) die('skip'); ?>
--CLEAN--
<?php
@unlink(__DIR__ . "/foobar.dtd");
--FILE--
<?php
chdir(__DIR__);
$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "foobar.dtd">
<foo>bar</foo>
XML;

$dtd = <<<DTD
<!ELEMENT foo (#PCDATA)>
DTD;


libxml_set_external_entity_loader(
	function ($public, $system, $context) {
		var_dump($public,$system);
		return null;
	}
);

$dd = new DOMDocument;
$r = $dd->loadXML($xml);
var_dump($dd->validate());

libxml_set_external_entity_loader(NULL);
file_put_contents(__DIR__ . "/foobar.dtd", $dtd);
var_dump($dd->validate());

echo "Done.\n";
--EXPECTF--
string(10) "-//FOO/BAR"
string(%d) "%sfoobar.dtd"

Warning: DOMDocument::validate(): Could not load the external subset "foobar.dtd" in %s on line %d
bool(false)
bool(true)
Done.
