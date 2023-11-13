--TEST--
DOM\HTMLDocument::createFromString() - line and column test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!doctype html>
<html>
    <head>
        <title>foo</title>
    </head>
    <body>
        <div id="mydiv" x="foo">
            <p>
                <strong>This is my paragraph</strong>
                <!-- my comment -->
            </p>
        </div>
    </body>
</html>
HTML);

$xpath = new DOMXPath($dom);

foreach ($xpath->query("//*") as $element) {
    echo "Element: '", $element->tagName, "', ", $element->getLineNo(), "\n";
}

foreach ($xpath->query("//*[name()='strong']") as $element) {
    echo "Text: '", $element->textContent, "', ", $element->firstChild->getLineNo(), "\n";
}

foreach ($xpath->query("//*[name()='div']") as $element) {
    foreach ($element->attributes as $attribute) {
        echo "Attribute: '", $attribute->nodeName, "', ", $attribute->getLineNo(), "\n";
    }
}

foreach ($xpath->query("//comment()") as $comment) {
    echo "Comment: '", $comment->data, "', ", $comment->getLineNo(), "\n";
}

?>
--EXPECT--
Element: 'html', 1
Element: 'head', 2
Element: 'title', 3
Element: 'body', 5
Element: 'div', 6
Element: 'p', 7
Element: 'strong', 8
Text: 'This is my paragraph', 8
Attribute: 'id', 6
Attribute: 'x', 6
Comment: ' my comment ', 9
