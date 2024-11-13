--TEST--
Dom\HTMLDocument::createFromString() with LIBXML_COMPACT
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
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

$xpath = new Dom\XPath($dom);
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
