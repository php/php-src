--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - set attribute in xml variation
--EXTENSIONS--
dom
--FILE--
<?php
function test($dom, $fn) {
    $test1 = $dom->getElementById('x');
    $test2 = $dom->getElementById('y');

    echo "--- After resetting test1's id ---\n";

    $fn($test1, 'xml:id', 'y');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test2's id ---\n";

    $fn($test2, 'xml:id', 'x');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test1's id ---\n";

    $fn($test1, 'xml:id', 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting test2's id ---\n";

    $fn($test2, 'xml:id', 'z');
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

echo "\n=== DOMDocument: setAttribute ===\n\n";

$dom = new DOMDocument;
$dom->loadXML($common_xml);
test($dom, fn ($element, $name, $value) => $element->setAttribute($name, $value));

echo "\n=== DOMDocument: setAttributeNS ===\n\n";

$dom = new DOMDocument;
$dom->loadXML($common_xml);
test($dom, fn ($element, $name, $value) => $element->setAttributeNS(getNamespace($name), $name, $value));

echo "\n=== Dom\\XMLDocument: setAttribute ===\n\n";

$dom = Dom\XMLDocument::createFromString($common_xml);
test($dom, fn ($element, $name, $value) => $element->setAttribute($name, $value));

echo "\n=== Dom\\XMLDocument: setAttributeNS ===\n\n";

$dom = Dom\XMLDocument::createFromString($common_xml);
test($dom, fn ($element, $name, $value) => $element->setAttributeNS(getNamespace($name), $name, $value));
?>
--EXPECT--
=== DOMDocument: setAttribute ===

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

=== DOMDocument: setAttributeNS ===

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

=== Dom\XMLDocument: setAttributeNS ===

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
