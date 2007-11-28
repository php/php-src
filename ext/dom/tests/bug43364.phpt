--TEST--
Bug #43364 (recursive xincludes don't remove internal xml nodes properly)
--FILE--
<?php 
function loopElements($nodes)
{
    $count = 0;
    foreach($nodes as $node) {
        if($node instanceof DOMElement) {
            $count++;
            if($node->childNodes->length > 0) {
                $count += loopElements($node->childNodes);
            }
        }
    }
    return $count;
}

$xml = <<<DOC
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:xi="http://www.w3.org/2001/XInclude">
    <a>
        <a_child1>ac1</a_child1>
        <a_child2>ac2</a_child2>
    </a>
    <b><xi:include xpointer="xpointer(/root/a)" /></b>
    <c><xi:include xpointer="xpointer(/root/b)" /></c>
</root>
DOC;

$doc = new DomDocument();
$doc->loadXml($xml);
$doc->xinclude();

$count = loopElements(array($doc->documentElement));

var_dump($count);
?>
--EXPECT--
int(13)
