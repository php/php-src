--TEST--
Dom\Element::getElementsByClassName() non quirks mode
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<div id="container">
    <p class="Bar">1</p>
    <p class="bar">2</p>
    <p class="Bar Foo">3</p>
    <p class="Bar foo">4</p>
    <p class="foo bar">5</p>
    <p class="foo bar" name="here">6</p>
</div>
<div>
    <p class="Bar">7</p>
    <p class="bar">8</p>
    <p class="Bar Foo">9</p>
    <p class="Bar foo">10</p>
    <p class="foo bar">11</p>
</div>
HTML);

$collection = $dom->getElementsByClassName("foo \n bar");

echo "There are {$collection->length} items in the document in total that have both \"foo\" and \"bar\"\n";

$collection = $dom->getElementById('container')->getElementsByClassName("foo \n bar");

echo "There are {$collection->length} items in #container in total that have both \"foo\" and \"bar\"\n";

foreach ($collection as $key => $node) {
    echo "--- Key $key ---\n";
    var_dump($node->tagName, $node->textContent);
    var_dump($node === $collection->item($key));
}

var_dump($collection->namedItem("here")->textContent);

?>
--EXPECT--
There are 3 items in the document in total that have both "foo" and "bar"
There are 2 items in #container in total that have both "foo" and "bar"
--- Key 0 ---
string(1) "P"
string(1) "5"
bool(true)
--- Key 1 ---
string(1) "P"
string(1) "6"
bool(true)
string(1) "6"
