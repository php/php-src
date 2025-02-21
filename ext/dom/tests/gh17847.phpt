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
        <p>garbage</p>
      </xi:include>
    </xi:test>
</root>
XML);

$xpath = new DOMXPath($doc);

$garbage = [];
foreach ($xpath->query('//p') as $entry)
    $garbage[] = $entry;

@$doc->xinclude();

var_dump($garbage);
?>
--EXPECT--
array(3) {
  [0]=>
  object(DOMElement)#3 (1) {
    ["schemaTypeInfo"]=>
    NULL
  }
  [1]=>
  object(DOMElement)#4 (1) {
    ["schemaTypeInfo"]=>
    NULL
  }
  [2]=>
  object(DOMElement)#5 (1) {
    ["schemaTypeInfo"]=>
    NULL
  }
}
