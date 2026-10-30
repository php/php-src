--TEST--
GH-21768: Assertion failure in ReflectionProperty::is{Readable,Writable}() on internal virtual properties
--EXTENSIONS--
dom
--FILE--
<?php

$rc = new ReflectionClass('DOMDocument');
foreach ($rc->getProperties() as $rp) {
    if (!$rp->isVirtual()) {
        continue;
    }
    echo $rp->getName(), ': readable=', var_export($rp->isReadable(null), true),
        ', writable=', var_export($rp->isWritable(null), true), PHP_EOL;
}
echo "done", PHP_EOL;

?>
--EXPECT--
nodeName: readable=true, writable=false
nodeValue: readable=true, writable=true
nodeType: readable=true, writable=false
parentNode: readable=true, writable=false
parentElement: readable=true, writable=false
childNodes: readable=true, writable=false
firstChild: readable=true, writable=false
lastChild: readable=true, writable=false
previousSibling: readable=true, writable=false
nextSibling: readable=true, writable=false
attributes: readable=true, writable=false
isConnected: readable=true, writable=false
ownerDocument: readable=true, writable=false
namespaceURI: readable=true, writable=false
prefix: readable=true, writable=true
localName: readable=true, writable=false
baseURI: readable=true, writable=false
textContent: readable=true, writable=true
doctype: readable=true, writable=false
implementation: readable=true, writable=false
documentElement: readable=true, writable=false
actualEncoding: readable=true, writable=false
encoding: readable=true, writable=true
xmlEncoding: readable=true, writable=false
standalone: readable=true, writable=true
xmlStandalone: readable=true, writable=true
version: readable=true, writable=true
xmlVersion: readable=true, writable=true
strictErrorChecking: readable=true, writable=true
documentURI: readable=true, writable=true
config: readable=true, writable=false
formatOutput: readable=true, writable=true
validateOnParse: readable=true, writable=true
resolveExternals: readable=true, writable=true
preserveWhiteSpace: readable=true, writable=true
recover: readable=true, writable=true
substituteEntities: readable=true, writable=true
firstElementChild: readable=true, writable=false
lastElementChild: readable=true, writable=false
childElementCount: readable=true, writable=false
done
