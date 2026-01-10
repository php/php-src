<?php

function test(?string $uri, string $qualifiedName) {
    $uri_readable = is_null($uri) ? 'NULL' : "\"$uri\"";
    echo "--- Testing $uri_readable, \"$qualifiedName\" ---\n";
    $d = new DOMDocument();
    $d->appendChild($d->createElement('root'));
    try {
        $attr = $d->createAttributeNS($uri, $qualifiedName);
        $d->documentElement->setAttributeNodeNS($attr);
        echo "Attr prefix: ";
        var_dump($attr->prefix);
        echo "Attr namespaceURI: ";
        var_dump($attr->namespaceURI);
        echo "Attr value: ";
        var_dump($attr->value);
        echo "root namespaceURI: ";
        var_dump($d->documentElement->namespaceURI);
        echo "Equality check: ";
        $parts = explode(':', $qualifiedName);
        var_dump($attr === $d->documentElement->getAttributeNodeNS($uri, $parts[count($parts) - 1]));
        echo $d->saveXML(), "\n";
    } catch (DOMException $e) {
        echo "Exception: ", $e->getMessage(), "\n\n";
    }
}
