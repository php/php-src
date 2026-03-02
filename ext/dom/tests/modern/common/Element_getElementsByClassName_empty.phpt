--TEST--
Dom\Element::getElementsByClassName() empty class names
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<div class="  foo bar ">
    <p id="child"></p>
</div>
HTML, LIBXML_NOERROR);

$collection = $dom->documentElement->getElementsByClassName("");
var_dump($collection->count());

foreach ($collection as $node) {
    throw new Error("unreachable");
}

var_dump($dom->getElementsByClassName(" ")->count());
var_dump($dom->getElementsByClassName("\t")->count());
var_dump($dom->getElementsByClassName("\t\n\f\v")->count());
var_dump($dom->getElementsByClassName("\t\n\f\v")->namedItem("child"));

?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
NULL
