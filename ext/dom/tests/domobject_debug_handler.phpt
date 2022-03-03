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
Deprecated: Creation of dynamic property DOMDocument::$dynamicProperty is deprecated in %s on line %d
DOMDocument Object
(
    [config] => 
    [dynamicProperty] => stdClass Object
        (
        )

    [doctype] => 
    [implementation] => (object value omitted)
    [documentElement] => (object value omitted)
    [actualEncoding] => 
    [encoding] => 
    [xmlEncoding] => 
    [standalone] => 1
    [xmlStandalone] => 1
    [version] => 1.0
    [xmlVersion] => 1.0
    [strictErrorChecking] => 1
    [documentURI] => %s
    [formatOutput] => 
    [validateOnParse] => 
    [resolveExternals] => 
    [preserveWhiteSpace] => 1
    [recover] => 
    [substituteEntities] => 
    [firstElementChild] => (object value omitted)
    [lastElementChild] => (object value omitted)
    [childElementCount] => 1
    [nodeName] => #document
    [nodeValue] => 
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
    [prefix] => 
    [localName] => 
    [baseURI] => %s
    [textContent] => 
    foobar

)
