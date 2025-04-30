--TEST--
Dom\HTMLDocument::createFromString() - line and column test
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
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

$xpath = new Dom\XPath($dom);

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
Element: 'HTML', 1
Element: 'HEAD', 2
Element: 'TITLE', 3
Element: 'BODY', 5
Element: 'DIV', 6
Element: 'P', 7
Element: 'STRONG', 8
Text: 'This is my paragraph', 8
Attribute: 'id', 6
Attribute: 'x', 6
Comment: ' my comment ', 9
