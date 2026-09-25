--TEST--
GH-23887 (Dom\HTMLCollection::namedItem() assertion failure, hang, or missed first element)
--CREDITS--
Lu Maltsis (lmaltsis)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<b id="container">

HTML);
var_dump($dom->getElementById('container')->getElementsByClassName('')->namedItem("here"));

$dom = Dom\HTMLDocument::createFromString('<!DOCTYPE html><div id="c"><p id="a" class="x"></p><p id="b" class="x"></p></div>', LIBXML_NOERROR);
$c = $dom->getElementById('c');

var_dump($c->getElementsByClassName('')->namedItem("a"));
var_dump($c->getElementsByClassName('x')->namedItem("a")->id);
var_dump($c->getElementsByClassName('x')->namedItem("b")->id);
var_dump($c->children->namedItem("a")->id);
var_dump($c->children->namedItem("b")->id);
var_dump($c->children->namedItem("c"));
var_dump($c->getElementsByTagName('p')->namedItem("a")->id);
var_dump($c->getElementsByTagName('p')->namedItem("b")->id);

?>
--EXPECT--
NULL
NULL
string(1) "a"
string(1) "b"
string(1) "a"
string(1) "b"
NULL
string(1) "a"
string(1) "b"
