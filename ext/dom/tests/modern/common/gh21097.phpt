--TEST--
GH-21097 (Accessing Dom\Node properties can can throw TypeError(s))
--EXTENSIONS--
dom
--CREDITS--
mbeccati
--FILE--
<?php

$implementation = new \Dom\Implementation();
$node = $implementation->createDocumentType('html', 'publicId', 'systemId');

$r = new \ReflectionClass($node);
foreach ($r->getProperties(\ReflectionProperty::IS_PUBLIC) as $p) {
    echo $p->getName(), ": ";
    var_dump($node->{$p->getName()});
}

?>
--EXPECT--
nodeType: int(10)
nodeName: string(4) "html"
baseURI: string(11) "about:blank"
isConnected: bool(false)
ownerDocument: NULL
parentNode: NULL
parentElement: NULL
childNodes: object(Dom\NodeList)#24 (1) {
  ["length"]=>
  int(0)
}
firstChild: NULL
lastChild: NULL
previousSibling: NULL
nextSibling: NULL
nodeValue: NULL
textContent: string(0) ""
name: string(4) "html"
entities: object(Dom\DtdNamedNodeMap)#24 (1) {
  ["length"]=>
  int(0)
}
notations: object(Dom\DtdNamedNodeMap)#24 (1) {
  ["length"]=>
  int(0)
}
publicId: string(8) "publicId"
systemId: string(8) "systemId"
internalSubset: NULL
