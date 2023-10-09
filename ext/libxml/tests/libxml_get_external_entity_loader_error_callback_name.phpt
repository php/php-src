--TEST--
libxml_get_external_entity_loader() display correct callable name
--EXTENSIONS--
dom
dba
--FILE--
<?php

$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

$dtd = <<<DTD
<!ELEMENT foo (#PCDATA)>
DTD;

class Handler {
	public function handle($public, $system, $context) {
	    $file = __DIR__ . '/db.dba';
		return dba_open($file, 'n');
	}
}

$o = new Handler();
libxml_set_external_entity_loader([$o, 'handle']);
$dd = new DOMDocument;
$r = $dd->loadXML($xml);

try {
    var_dump($dd->validate());
} catch (\Throwable $e) {
    var_dump($e->getMessage());
}

?>
--CLEAN--
<?php
$file = __DIR__ . '/db.dba';
unlink($file);
?>
--EXPECT--
string(73) "DOMDocument::validate(): supplied resource is not a valid stream resource"
