--TEST--
Test reading Element::${inner,outer}HTML on XML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

function createContainer() {
    global $dom;
    $element = $dom->createElement("container");
    return $element;
}

$container = createContainer();
$container->append("Hello, world!");
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$container = createContainer();
$container->append($dom->createComment("This is -a- comment"));
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$container = createContainer();
// Note: intentionally typo'd to check whether the string matching against "xml" happens correctly
//       i.e. no bugs with prefix-matching only.
$container->append($dom->createProcessingInstruction("xmll", ""));
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$container = createContainer();
$container->append($dom->createProcessingInstruction("almostmalformed", ">?"));
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns', 'http://example.com/');
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttributeNS('urn:a', 'name', '');
$element->setAttributeNS('urn:b', 'name', '');
var_dump($container->innerHTML);
var_dump($container->outerHTML);

$dom = DOM\XMLDocument::createFromFile(__DIR__ . '/../../book.xml');
var_dump($dom->documentElement->innerHTML);
var_dump($dom->documentElement->outerHTML);

?>
--EXPECT--
string(13) "Hello, world!"
string(36) "<container>Hello, world!</container>"
string(26) "<!--This is -a- comment-->"
string(49) "<container><!--This is -a- comment--></container>"
string(9) "<?xmll ?>"
string(32) "<container><?xmll ?></container>"
string(22) "<?almostmalformed >??>"
string(45) "<container><?almostmalformed >??></container>"
string(12) "<container/>"
string(35) "<container><container/></container>"
string(72) "<container xmlns:ns1="urn:a" ns1:name="" xmlns:ns2="urn:b" ns2:name=""/>"
string(95) "<container><container xmlns:ns1="urn:a" ns1:name="" xmlns:ns2="urn:b" ns2:name=""/></container>"
string(167) "
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
"
string(182) "<books>
 <book>
  <title>The Grapes of Wrath</title>
  <author>John Steinbeck</author>
 </book>
 <book>
  <title>The Pearl</title>
  <author>John Steinbeck</author>
 </book>
</books>"
