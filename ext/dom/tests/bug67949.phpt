--TEST--
Bug #67949: DOMNodeList elements should be accessible through array notation
--FILE--
<?php

$html = <<<HTML
<div>data</div>
HTML;
$doc = new DOMDocument;
$doc->loadHTML($html);
var_dump($doc->getElementsByTagName('div')[0]->textContent);
var_dump($doc->getElementsByTagName('div')['test']->textContent); // testing that weak casting works
var_dump(isset($doc->getElementsByTagName('div')[0]));
var_dump(isset($doc->getElementsByTagName('div')[1]));

--EXPECT--
string(4) "data"
string(4) "data"
bool(true)
bool(false)
