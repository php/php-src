--TEST--
GH-11625 (DOMElement::replaceWith() doesn't replace node with DOMDocumentFragment but just deletes node or causes wrapping <></> depending on libxml2 version)
--EXTENSIONS--
dom
--FILE--
<?php

function test($mutator) {
    $html = <<<XML
    <body>
        <div></div><div></div>
    </body>
    XML;

    $dom = new DOMDocument();
    $dom->loadXML($html);

    $divs = iterator_to_array($dom->getElementsByTagName('div')->getIterator());
    $i = 0;
    foreach ($divs as $div) {
        $mutator($dom, $div, $i);
        echo $dom->saveHTML();
        $i++;
    }
}

echo "--- Single replacement ---\n";

test(function($dom, $div, $i) {
    $fragment = $dom->createDocumentFragment();
    $fragment->appendXML("<p>Hi $i!</p>");
    $div->replaceWith($fragment);
});

echo "--- Multiple replacement ---\n";

test(function($dom, $div, $i) {
    $fragment = $dom->createDocumentFragment();
    $fragment->appendXML("<p>Hi $i!</p>");
    $div->replaceWith($fragment, $dom->createElement('x'), "hello");
});

echo "--- Empty fragment replacement ---\n";

test(function($dom, $div, $i) {
    $fragment = $dom->createDocumentFragment();
    $div->replaceWith($fragment);
});

?>
--EXPECT--
--- Single replacement ---
<body>
    <p>Hi 0!</p><div></div>
</body>
<body>
    <p>Hi 0!</p><p>Hi 1!</p>
</body>
--- Multiple replacement ---
<body>
    <p>Hi 0!</p><x></x>hello<div></div>
</body>
<body>
    <p>Hi 0!</p><x></x>hello<p>Hi 1!</p><x></x>hello
</body>
--- Empty fragment replacement ---
<body>
    <div></div>
</body>
<body>
    
</body>
