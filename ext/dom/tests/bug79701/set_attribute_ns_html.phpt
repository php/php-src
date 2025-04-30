--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - set attribute ns in html variation
--EXTENSIONS--
dom
--FILE--
<?php
function test($namespace) {
    $dom = Dom\HTMLDocument::createFromString(<<<HTML
    <p>
        <em id="x">1</em>
        <strong id="y">2</strong>
    </p>
    HTML, LIBXML_NOERROR);

    $test1 = $dom->getElementById('x');
    $test2 = $dom->getElementById('y');

    echo "--- After resetting em's id ---\n";

    $test1->setAttributeNS($namespace, "id", 'y');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting strong's id ---\n";

    $test2->setAttributeNS($namespace, "id", 'x');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting em's id ---\n";

    $test1->setAttributeNS($namespace, "id", 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting strong's id ---\n";

    $test2->setAttributeNS($namespace, "id", 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- Get id z ---\n";

    var_dump($dom->getElementById('z')?->nodeName);
}

echo "=== Test empty namespace ===\n\n";
test("");
echo "\n=== Test \"urn:x\" namespace ===\n\n";
test("urn:x");
?>
--EXPECT--
=== Test empty namespace ===

--- After resetting em's id ---
NULL
string(2) "EM"
--- After resetting strong's id ---
string(6) "STRONG"
string(2) "EM"
--- After resetting em's id ---
string(6) "STRONG"
NULL
--- After resetting strong's id ---
NULL
NULL
--- Get id z ---
string(2) "EM"

=== Test "urn:x" namespace ===

--- After resetting em's id ---
string(2) "EM"
string(6) "STRONG"
--- After resetting strong's id ---
string(2) "EM"
string(6) "STRONG"
--- After resetting em's id ---
string(2) "EM"
string(6) "STRONG"
--- After resetting strong's id ---
string(2) "EM"
string(6) "STRONG"
--- Get id z ---
NULL
