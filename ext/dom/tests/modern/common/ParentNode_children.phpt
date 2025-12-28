--TEST--
ParentNode::$children
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<root>
    <a/>
    <b/>
    <c id="foo"><c1/><!-- comment --><c2/></c>
    <?garbage ?>
    <e/>
</root>
XML);
$children = $dom->documentElement->children;
var_dump($children === $dom->documentElement->children); // Tests caching behaviour
var_dump($children !== (clone $dom->documentElement)->children); // Tests caching behaviour does not persist across clones
var_dump(count($children));
var_dump($children->length);

foreach ($children as $key => $child) {
    var_dump($key, $child->nodeName);
}

foreach ($children->namedItem('foo')->children as $key => $child) {
    var_dump($key, $child->nodeName);
}

?>
--EXPECT--
bool(true)
bool(true)
int(4)
int(4)
int(0)
string(1) "a"
int(1)
string(1) "b"
int(2)
string(1) "c"
int(3)
string(1) "e"
int(0)
string(2) "c1"
int(1)
string(2) "c2"
