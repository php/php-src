--TEST--
<template> element renaming
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
$dom = Dom\HTMLDocument::createFromString($html);
$template = $dom->body->firstElementChild;
var_dump($template->innerHTML);

$template->rename($template->namespaceURI, 'screwthis');
var_dump($template->innerHTML);
$template->rename($template->namespaceURI, 'template');
var_dump($template->innerHTML);

?>
--EXPECT--
string(16) "a<div>foo</div>b"
string(0) ""
string(0) ""
