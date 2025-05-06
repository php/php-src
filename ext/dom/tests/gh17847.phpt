--TEST--
GH-17847 (xinclude destroys live node)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML(<<<XML
<root>
    <xi:include href="thisisnonexistent" xmlns:xi="http://www.w3.org/2001/XInclude">
        <xi:fallback>fallback<p>garbage</p></xi:fallback>
        <p>garbage</p>
    </xi:include>
    <xi:test xmlns:xi="http://www.w3.org/2001/XInclude">
      <xi:include href="thisisnonexistent">
        <p attr="foo" attr2="bar">garbage</p>
      </xi:include>
    </xi:test>
</root>
XML);

$xpath = new DOMXPath($doc);

$garbage = [];
foreach ($xpath->query('//p') as $entry) {
    $garbage[] = $entry;
    foreach ($entry->attributes as $attr) {
        $garbage[] = $attr;
        foreach ($attr->childNodes as $child) {
            $garbage[] = $child;
        }
    }
}

@$doc->xinclude();

foreach ($garbage as $node) {
    try {
        var_dump($node->localName);
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Invalid State Error
Invalid State Error
Invalid State Error
Invalid State Error
Invalid State Error
Invalid State Error
Invalid State Error
