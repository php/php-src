--TEST--
Bug #79701 (getElementById does not correctly work with duplicate definitions) - id property variation
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createFromString(<<<XML
<root>
    <test1/>
    <test2/>
</root>
XML);

$test1 = $dom->documentElement->firstElementChild;
$test2 = $test1->nextElementSibling;

echo "--- After parsing ---\n";
var_dump($dom->getElementById("x")?->nodeName);

echo "--- After setting test2 ---\n";
$test2->id = "x";
var_dump($dom->getElementById("x")?->nodeName);
echo "--- After setting test1 ---\n";
$test1->id = "x";
var_dump($dom->getElementById("x")?->nodeName);
echo "--- After resetting test1 ---\n";
$test1->id = "y";
var_dump($dom->getElementById("x")?->nodeName);
echo "--- After resetting test2 ---\n";
$test2->id = "y";
var_dump($dom->getElementById("x")?->nodeName);
echo "--- After resetting test1 ---\n";
$test1->id = "x";
var_dump($dom->getElementById("x")?->nodeName);
echo "--- After calling setIdAttribute with false on test1 ---\n";
$test1->setIdAttribute("id", false);
var_dump($dom->getElementById("x")?->nodeName);
?>
--EXPECT--
--- After parsing ---
NULL
--- After setting test2 ---
string(5) "test2"
--- After setting test1 ---
string(5) "test1"
--- After resetting test1 ---
string(5) "test2"
--- After resetting test2 ---
NULL
--- After resetting test1 ---
string(5) "test1"
--- After calling setIdAttribute with false on test1 ---
NULL
