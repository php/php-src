--TEST--
Objects of DOM extension: debug info object handler.
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<foo>
    <bar>foobar</bar>
</foo>
XML;
$d = new domdocument;
$d->dynamicProperty = new stdclass;
$d->loadXML($xml);
print_r($d);
?>
--EXPECTF--
DOMDocument Object
(
    [nodeValue] => 
    [prefix] => 
    [textContent] => 
    foobar

    [encoding] => 
    [standalone] => 1
    [xmlStandalone] => 1
    [version] => 1.0
    [xmlVersion] => 1.0
    [strictErrorChecking] => 1
    [documentURI] => %s
    [config] => 
    [formatOutput] => 
    [validateOnParse] => 
    [resolveExternals] => 
    [preserveWhiteSpace] => 1
    [recover] => 
    [substituteEntities] => 
    [dynamicProperty] => stdClass Object
        (
        )

    [doctype] => 
    [implementation] => (object value omitted)
    [documentElement] => (object value omitted)
    [actualEncoding] => 
    [xmlEncoding] => 
    [firstElementChild] => (object value omitted)
    [lastElementChild] => (object value omitted)
    [childElementCount] => 1
    [nodeName] => #document
    [nodeType] => 9
    [parentNode] => 
    [childNodes] => (object value omitted)
    [firstChild] => (object value omitted)
    [lastChild] => (object value omitted)
    [previousSibling] => 
    [nextSibling] => 
    [attributes] => 
    [ownerDocument] => 
    [namespaceURI] => 
    [localName] => 
    [baseURI] => %s
)
