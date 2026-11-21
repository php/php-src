--TEST--
libxml_set_external_entity_loader(): the loader may swap itself out for a nested parse
--EXTENSIONS--
dom
--FILE--
<?php
$loader = null;
$loader = function ($public, $system, $context) use (&$loader) {
    libxml_set_external_entity_loader(null);
    $inner = new DOMDocument;
    $inner->loadXML('<inner/>');
    echo $inner->documentElement->nodeName, PHP_EOL;
    libxml_set_external_entity_loader($loader);

    $f = fopen('php://temp', 'r+');
    fwrite($f, '<!ELEMENT foo (#PCDATA)>');
    rewind($f);
    return $f;
};

libxml_set_external_entity_loader($loader);

$xml = <<<XML
<!DOCTYPE foo PUBLIC "-//FOO/BAR" "http://example.com/foobar">
<foo>bar</foo>
XML;

$dd = new DOMDocument;
$dd->loadXML($xml);
var_dump($dd->validate());

libxml_set_external_entity_loader(null);
var_dump(libxml_get_external_entity_loader());

echo 'Done.', PHP_EOL;
?>
--EXPECT--
inner
bool(true)
NULL
Done.
