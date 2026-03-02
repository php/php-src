--TEST--
<template> element no default namespace
--EXTENSIONS--
dom
--FILE--
<?php

$html = <<<HTML
<!DOCTYPE html>
<html>
    <body>
        <template>a<div>foo</div>b</template>
    </body>
</html>
HTML;
$dom = Dom\HTMLDocument::createFromString($html, Dom\HTML_NO_DEFAULT_NS);
$template = $dom->getElementsByTagName('template')[0];
var_dump($template->innerHTML);
var_dump($template->firstElementChild->tagName);

?>
--EXPECT--
string(16) "a<div>foo</div>b"
string(3) "div"
