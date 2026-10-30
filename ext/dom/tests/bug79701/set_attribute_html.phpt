--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - set attribute in html variation
--EXTENSIONS--
dom
--FILE--
<?php
foreach (["xml:id", "ID"] as $name) {
    $dom = Dom\HTMLDocument::createFromString(<<<HTML
    <p>
        <em id="x">1</em>
        <strong id="y">2</strong>
    </p>
    HTML, LIBXML_NOERROR);

    echo "\n=== Attribute name $name ===\n\n";

    $test1 = $dom->getElementById('x');
    $test2 = $dom->getElementById('y');

    echo "--- After resetting em's id ---\n";

    $test1->setAttribute($name, 'y');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting strong's id ---\n";

    $test2->setAttribute($name, 'x');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting em's id ---\n";

    $test1->setAttribute($name, 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- After resetting strong's id ---\n";

    $test2->setAttribute($name, 'z');
    var_dump($dom->getElementById('x')?->nodeName);
    var_dump($dom->getElementById('y')?->nodeName);

    echo "--- Get id z ---\n";

    var_dump($dom->getElementById('z')?->nodeName);
}
?>
--EXPECT--
=== Attribute name xml:id ===

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

=== Attribute name ID ===

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
