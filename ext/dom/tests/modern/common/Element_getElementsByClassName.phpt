--TEST--
Dom\Element::getElementsByClassName()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<div class="  foo bar ">
    <p class="bar">
        <p class="bar" name="here">1</p>
        <p name="here">2</p>
        <p name="here">3</p>
        <p class="bar" name="here">4</p>
    </p>
    <b class="foo bars"></b>
</div>
HTML, LIBXML_NOERROR);
$collection = $dom->documentElement->getElementsByClassName("bar");

echo "There are {$dom->getElementsByClassName("foo \n bar")->count()} items in the document in total that have both \"foo\" and \"bar\"\n";

echo "There are {$collection->count()} \"bar\" items\n";

foreach ($collection as $key => $node) {
    var_dump($key, $node->tagName);
    var_dump($node === $collection->item($key));
}

var_dump($collection->namedItem("here")->textContent);

?>
--EXPECT--
There are 1 items in the document in total that have both "foo" and "bar"
There are 4 "bar" items
int(0)
string(3) "DIV"
bool(false)
int(1)
string(1) "P"
bool(false)
string(1) "1"
