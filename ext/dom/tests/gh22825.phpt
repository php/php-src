--TEST--
GH-22825 (DOMElement::setAttribute() reaches EMPTY_SWITCH_DEFAULT_CASE() with DTD #FIXED default attributes)
--EXTENSIONS--
dom
--FILE--
<?php
$cases = [
    ['<!ATTLIST root A CDATA #FIXED "d">', '<root/>', 'A'],
    ['<!ATTLIST root A CDATA "d">', '<root/>', 'A'],
    ['<!ATTLIST root p:A CDATA #FIXED "d">', '<root xmlns:p="urn:p"/>', 'p:A'],
];

function element(string $attlist, string $root): DOMElement {
    $doc = new DOMDocument();
    $doc->loadXML("<!DOCTYPE root [$attlist]>$root");
    return $doc->documentElement;
}

foreach ($cases as [$attlist, $root, $name]) {
    echo "--- $attlist ---\n";

    $el = element($attlist, $root);
    echo "hasAttribute: ";
    var_dump($el->hasAttribute($name));
    echo "getAttribute: ";
    var_dump($el->getAttribute($name));

    $el = element($attlist, $root);
    $result = $el->setAttribute($name, 'v');
    echo "setAttribute: ", is_object($result) ? $result::class : var_export($result, true), "\n";
    echo "after setAttribute: ", trim($el->ownerDocument->saveXML($el)), "\n";

    $el = element($attlist, $root);
    echo "removeAttribute: ";
    var_dump($el->removeAttribute($name));
    echo "still present after removeAttribute: ";
    var_dump($el->hasAttribute($name));

    $el = element($attlist, $root);
    echo "toggleAttribute(false): ";
    var_dump($el->toggleAttribute($name, false));
    echo "still present after toggleAttribute(false): ";
    var_dump($el->hasAttribute($name));

    $el = element($attlist, $root);
    echo "toggleAttribute(true): ";
    var_dump($el->toggleAttribute($name, true));
    echo "still present after toggleAttribute(true): ";
    var_dump($el->hasAttribute($name));

    $el = element($attlist, $root);
    $attr = $el->ownerDocument->createAttribute($name);
    $attr->value = 'z';
    echo "setAttributeNode: ";
    var_dump($el->setAttributeNode($attr));
    echo "after setAttributeNode: ", trim($el->ownerDocument->saveXML($el)), "\n";
}
?>
--EXPECT--
--- <!ATTLIST root A CDATA #FIXED "d"> ---
hasAttribute: bool(true)
getAttribute: string(1) "d"
setAttribute: DOMAttr
after setAttribute: <root A="v"/>
removeAttribute: bool(false)
still present after removeAttribute: bool(true)
toggleAttribute(false): bool(true)
still present after toggleAttribute(false): bool(true)
toggleAttribute(true): bool(true)
still present after toggleAttribute(true): bool(true)
setAttributeNode: NULL
after setAttributeNode: <root A="z"/>
--- <!ATTLIST root A CDATA "d"> ---
hasAttribute: bool(true)
getAttribute: string(1) "d"
setAttribute: DOMAttr
after setAttribute: <root A="v"/>
removeAttribute: bool(false)
still present after removeAttribute: bool(true)
toggleAttribute(false): bool(true)
still present after toggleAttribute(false): bool(true)
toggleAttribute(true): bool(true)
still present after toggleAttribute(true): bool(true)
setAttributeNode: NULL
after setAttributeNode: <root A="z"/>
--- <!ATTLIST root p:A CDATA #FIXED "d"> ---
hasAttribute: bool(true)
getAttribute: string(1) "d"
setAttribute: DOMAttr
after setAttribute: <root xmlns:p="urn:p" p:A="v"/>
removeAttribute: bool(false)
still present after removeAttribute: bool(true)
toggleAttribute(false): bool(true)
still present after toggleAttribute(false): bool(true)
toggleAttribute(true): bool(true)
still present after toggleAttribute(true): bool(true)
setAttributeNode: NULL
after setAttributeNode: <root xmlns:p="urn:p" p:A="z"/>
