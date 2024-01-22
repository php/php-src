--TEST--
Test DOM\HTMLDocument::getElementsByTagName(NS)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>Test</title>
</head>
<body>
    <h1>Test</h1>
    <p>Test</p>
    <svg width="100" height="100">
        <circle cx="0" cy="0" r="10"/>
    </svg>
    <math>
        <mtable id="table"></mtable>
    </math>
</body>
</html>
HTML);

echo "--- getElementsByTagName ---\n";

var_dump($dom->getElementsByTagName("p")[0]?->nodeName);
var_dump($dom->getElementsByTagName("math")[0]?->nodeName);
var_dump($dom->getElementsByTagName("mtable")[0]?->nodeName);
var_dump($dom->getElementsByTagName("svg")[0]?->nodeName);
var_dump($dom->getElementsByTagName("circle")[0]?->nodeName);

echo "--- getElementsByTagNameNS (*) ---\n";

var_dump($dom->getElementsByTagNameNS("*", "p")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("*", "math")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("*", "mtable")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("*", "svg")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("*", "circle")[0]?->nodeName);

echo "--- getElementsByTagNameNS (xhtml) ---\n";

var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1999/xhtml", "p")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1999/xhtml", "math")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1999/xhtml", "mtable")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1999/xhtml", "svg")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1999/xhtml", "circle")[0]?->nodeName);

echo "--- getElementsByTagNameNS (svg) ---\n";

var_dump($dom->getElementsByTagNameNS("http://www.w3.org/2000/svg", "p")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/2000/svg", "math")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/2000/svg", "mtable")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/2000/svg", "svg")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/2000/svg", "circle")[0]?->nodeName);

echo "--- getElementsByTagNameNS (math) ---\n";

var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1998/Math/MathML", "p")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1998/Math/MathML", "math")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1998/Math/MathML", "mtable")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1998/Math/MathML", "svg")[0]?->nodeName);
var_dump($dom->getElementsByTagNameNS("http://www.w3.org/1998/Math/MathML", "circle")[0]?->nodeName);

?>
--EXPECT--
--- getElementsByTagName ---
string(1) "p"
string(4) "math"
string(6) "mtable"
string(3) "svg"
string(6) "circle"
--- getElementsByTagNameNS (*) ---
string(1) "p"
string(4) "math"
string(6) "mtable"
string(3) "svg"
string(6) "circle"
--- getElementsByTagNameNS (xhtml) ---
string(1) "p"
NULL
NULL
NULL
NULL
--- getElementsByTagNameNS (svg) ---
NULL
NULL
NULL
string(3) "svg"
string(6) "circle"
--- getElementsByTagNameNS (math) ---
NULL
string(4) "math"
string(6) "mtable"
NULL
NULL
