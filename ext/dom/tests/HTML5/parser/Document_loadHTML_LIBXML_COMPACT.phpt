--TEST--
HTML5Document::loadHTML() with LIBXML_COMPACT
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML(<<<HTML
<!DOCTYPE HTML>
<html>
<head>
</head>
<body>
    <p> x </p>
    <p>foo</p>
    <p>foox</p>
    <p>fooxx</p>
    <p>fooxxx</p>
    <p>fooxxxx</p>
    <p>fooxxxxx</p>
    <p>this does not fit</p>
</body>
</html>
HTML, LIBXML_COMPACT);

$xpath = new DOMXPath($dom);
foreach ($xpath->query("//*[name()='p']") as $p) {
    echo $p->textContent, "\n";
}

?>
--EXPECT--
x 
foo
foox
fooxx
fooxxx
fooxxxx
fooxxxxx
this does not fit
