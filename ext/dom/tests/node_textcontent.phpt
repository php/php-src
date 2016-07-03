--TEST--
Testing reading and writing to DOMNode::textContent
--SKIPIF--
<?php
require_once('skipif.inc');
if (LIBXML_VERSION < 20707) die ('skip requires libxml2-2.7.7 or higher');
?>
--FILE--
<?php

$html = <<<HTML
<div id="test"><span>hi there</span></div>
HTML;

$text = '<p>hello world &trade;</p>';

$dom = new DOMDocument('1.0', 'UTF-8');
$dom->loadHTML($html);

$node = $dom->getElementById('test');
var_dump($node->textContent);
$node->textContent = $text;
var_dump($node->textContent == $text);

var_dump($dom->saveHTML($node));

?>
--EXPECT--
string(8) "hi there"
bool(true)
string(63) "<div id="test">&lt;p&gt;hello world &amp;trade;&lt;/p&gt;</div>"

