--TEST--
XPath: with a context node
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();
$dom->loadXML(<<<XML
<root>
    <child>
        <p>bar</p>
    </child>
    <child>
        <p>foo1</p>
        <p>foo2</p>
    </child>
</root>
XML);
$xpath = new DOMXpath($dom);

foreach ($xpath->query("p", $dom->documentElement->firstElementChild->nextElementSibling) as $p) {
    echo $p->textContent, "\n";
}

var_dump($xpath->evaluate("count(p)", $dom->documentElement->firstElementChild->nextElementSibling));

?>
--EXPECT--
foo1
foo2
float(2)
