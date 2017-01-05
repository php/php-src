--TEST--
libxml_set_external_entity_loader() basic test
--SKIPIF--
<?php if (!extension_loaded('dom')) die('skip'); ?>
--FILE--
<?php
$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

$dtd = <<<DTD
<!ELEMENT foo (#PCDATA)>
DTD;

libxml_set_external_entity_loader(
	function ($public, $system, $context) use($dtd){
		var_dump($public);
		var_dump($system);
		var_dump($context);
		$f = fopen("php://temp", "r+");
		fwrite($f, $dtd);
		rewind($f);
		return $f;
	}
);

$dd = new DOMDocument;
$r = $dd->loadXML($xml);
var_dump($dd->validate());

echo "Done.\n";

?>
--EXPECT--
string(10) "-//FOO/BAR"
string(25) "http://example.com/foobar"
array(4) {
  ["directory"]=>
  NULL
  ["intSubName"]=>
  NULL
  ["extSubURI"]=>
  NULL
  ["extSubSystem"]=>
  NULL
}
bool(true)
Done.
