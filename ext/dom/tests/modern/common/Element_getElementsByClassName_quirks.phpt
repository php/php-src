--TEST--
Dom\Element::getElementsByClassName() quirks mode
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<div class="  foo bar ">
    <main class="bar">
        <p name="here">1</p>
        <p class="bar" name="here">2</p>
        <p name="here">3</p>
        <p class="bAR" name="here">4</p>
    </main>
    <b class="foo bars"></b>
</div>
HTML, LIBXML_NOERROR);
$collection = $dom->documentElement->getElementsByClassName("Bar");

echo "There are {$dom->getElementsByClassName("foo \n bar")->count()} items in the document in total that have both \"foo\" and \"bar\"\n";

echo "There are {$collection->count()} \"Bar\" items\n";

foreach ($collection as $key => $node) {
    echo "--- Key $key ---\n";
    var_dump($node->tagName, $node->textContent);
    var_dump($node === $collection->item($key));
}

echo "--- named item \"here\" ---\n";

var_dump($collection->namedItem("here")->textContent);

?>
--EXPECT--
There are 1 items in the document in total that have both "foo" and "bar"
There are 4 "Bar" items
--- Key 0 ---
string(3) "DIV"
string(56) "
    
        1
        2
        3
        4
    
    
"
bool(true)
--- Key 1 ---
string(4) "MAIN"
string(45) "
        1
        2
        3
        4
    "
bool(true)
--- Key 2 ---
string(1) "P"
string(1) "2"
bool(true)
--- Key 3 ---
string(1) "P"
string(1) "4"
bool(true)
--- named item "here" ---
string(1) "2"
