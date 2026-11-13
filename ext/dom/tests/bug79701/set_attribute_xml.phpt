--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - set attribute in xml variation
--EXTENSIONS--
dom
--FILE--
<?php
function test($dom, $prefix, $fn) {
    $test1 = $dom->getElementById('x');
    $test2 = $dom->getElementById('y');

    echo "--- After resetting test1's id ---\n";

    $fn($test1, $prefix . 'id', 'y');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test2's id ---\n";

    $fn($test2, $prefix . 'id', 'x');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test1's id ---\n";

    $fn($test1, $prefix . 'id', 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test2's id ---\n";

    $fn($test2, $prefix . 'id', 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- Get id z ---\n";

    var_dump($dom->getElementById('z')?->nodeName);
}

function getNamespace($name) {
    if (str_contains($name, 'xml:')) {
        return 'http://www.w3.org/XML/1998/namespace';
    }
    return '';
}

$common_xml = <<<XML
<root>
  <test1 xml:id="x"/>
  <test2 xml:id="y"/>
</root>
XML;

echo "\n=== DOMDocument: setAttribute (prefixed) ===\n\n";

$dom = new DOMDocument;
$dom->loadXML($common_xml);
test($dom, "xml:", fn ($element, $name, $value) => $element->setAttribute($name, $value));

echo "\n=== DOMDocument: setAttributeNS (prefixed) ===\n\n";

$dom = new DOMDocument;
$dom->loadXML($common_xml);
test($dom, "xml:", fn ($element, $name, $value) => $element->setAttributeNS(getNamespace($name), $name, $value));

echo "\n=== Dom\\XMLDocument: setAttribute (prefixed) ===\n\n";

$dom = Dom\XMLDocument::createFromString($common_xml);
test($dom, "xml:", fn ($element, $name, $value) => $element?->setAttribute($name, $value));

echo "\n=== Dom\\XMLDocument: setAttributeNS (prefixed) ===\n\n";

$dom = Dom\XMLDocument::createFromString($common_xml);
test($dom, "xml:", fn ($element, $name, $value) => $element?->setAttribute(getNamespace($name), $name, $value));

echo "\n=== Dom\\XMLDocument: setAttribute ===\n\n";

$common_xml = <<<XML
<root>
  <test1 id="x"/>
  <test2 id="y"/>
</root>
XML;

$dom = Dom\XMLDocument::createFromString($common_xml);
test($dom, "", fn ($element, $name, $value) => $element?->setAttribute($name, $value));

?>
--EXPECT--
=== DOMDocument: setAttribute (prefixed) ===

--- After resetting test1's id ---
NULL
string(5) "test1"
--- After resetting test2's id ---
string(5) "test2"
string(5) "test1"
--- After resetting test1's id ---
string(5) "test2"
NULL
--- After resetting test2's id ---
NULL
NULL
--- Get id z ---
string(5) "test1"

=== DOMDocument: setAttributeNS (prefixed) ===

--- After resetting test1's id ---
NULL
string(5) "test1"
--- After resetting test2's id ---
string(5) "test2"
string(5) "test1"
--- After resetting test1's id ---
string(5) "test2"
NULL
--- After resetting test2's id ---
NULL
NULL
--- Get id z ---
string(5) "test1"

=== Dom\XMLDocument: setAttribute (prefixed) ===

--- After resetting test1's id ---
NULL
NULL
--- After resetting test2's id ---
NULL
NULL
--- After resetting test1's id ---
NULL
NULL
--- After resetting test2's id ---
NULL
NULL
--- Get id z ---
NULL

=== Dom\XMLDocument: setAttributeNS (prefixed) ===

--- After resetting test1's id ---
NULL
NULL
--- After resetting test2's id ---
NULL
NULL
--- After resetting test1's id ---
NULL
NULL
--- After resetting test2's id ---
NULL
NULL
--- Get id z ---
NULL

=== Dom\XMLDocument: setAttribute ===

--- After resetting test1's id ---
NULL
string(5) "test1"
--- After resetting test2's id ---
string(5) "test2"
string(5) "test1"
--- After resetting test1's id ---
string(5) "test2"
NULL
--- After resetting test2's id ---
NULL
NULL
--- Get id z ---
string(5) "test1"
