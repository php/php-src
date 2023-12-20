--TEST--
HTMLDocument::createFromString() with namespaced attributes
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
    <svg width="1" xmlns:xlink='http://www.w3.org/1999/xlink'>
        <use xlink:href='#test'></use>
    </svg>
</html>
HTML);

foreach (['svg', 'use'] as $tag) {
    $el = $dom->getElementsByTagName($tag)[0];
    foreach ($el->attributes as $attribute) {
        echo "Attribute: \n";
        var_dump($attribute->name, $attribute->value, $attribute->namespaceURI);
    }
}

?>
--EXPECT--
Attribute: 
string(5) "width"
string(1) "1"
NULL
Attribute: 
string(11) "xmlns:xlink"
string(28) "http://www.w3.org/1999/xlink"
string(29) "http://www.w3.org/2000/xmlns/"
Attribute: 
string(10) "xlink:href"
string(5) "#test"
string(28) "http://www.w3.org/1999/xlink"
