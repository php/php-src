--TEST--
HTMLDocument: Predefined namespaces
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
    <svg width="100" height="100" viewBox="0 0 4 2">
        <rect id="rectangle" x="10" y="20" width="90" height="60">
        </rect>
    </svg>
    <div>
        <p>foo</p>
    </div>
    <math>
        <!-- svg should be in the mathml namespace -->
        <mtable id="table"><svg></svg></mtable>
    </math>
</body>
</html>
HTML);

echo "--- Namespaces ---\n";
$xpath = new DOM\XPath($dom);
foreach ($xpath->query("//*[name()='body']//*") as $node) {
    echo $node->nodeName, " ", $node->namespaceURI ?? "(NONE)", "\n";
    foreach ($node->attributes as $attribute) {
        echo "  Attribute: ", $attribute->nodeName, " ", $attribute->namespaceURI ?? "(NONE)", "\n";
    }
}

echo "--- HTML serialization ---\n";
echo $dom->saveHTML(), "\n";
echo "--- XML serialization ---\n";
echo $dom->saveXML();

?>
--EXPECT--
--- Namespaces ---
svg http://www.w3.org/2000/svg
  Attribute: width (NONE)
  Attribute: height (NONE)
  Attribute: viewbox (NONE)
rect http://www.w3.org/2000/svg
  Attribute: id (NONE)
  Attribute: x (NONE)
  Attribute: y (NONE)
  Attribute: width (NONE)
  Attribute: height (NONE)
DIV http://www.w3.org/1999/xhtml
P http://www.w3.org/1999/xhtml
math http://www.w3.org/1998/Math/MathML
mtable http://www.w3.org/1998/Math/MathML
  Attribute: id (NONE)
svg http://www.w3.org/1998/Math/MathML
--- HTML serialization ---
<!DOCTYPE html><html><head>
    <title>Test</title>
</head>
<body>
    <svg width="100" height="100" viewbox="0 0 4 2">
        <rect id="rectangle" x="10" y="20" width="90" height="60">
        </rect>
    </svg>
    <div>
        <p>foo</p>
    </div>
    <math>
        <!-- svg should be in the mathml namespace -->
        <mtable id="table"><svg></svg></mtable>
    </math>

</body></html>
--- XML serialization ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head>
    <title>Test</title>
</head>
<body>
    <svg xmlns="http://www.w3.org/2000/svg" width="100" height="100" viewbox="0 0 4 2">
        <rect id="rectangle" x="10" y="20" width="90" height="60">
        </rect>
    </svg>
    <div>
        <p>foo</p>
    </div>
    <math xmlns="http://www.w3.org/1998/Math/MathML">
        <!-- svg should be in the mathml namespace -->
        <mtable id="table"><svg/></mtable>
    </math>

</body></html>
