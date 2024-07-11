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

try {
    $template->rename($template->namespaceURI, 'screwthis');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

// These shouldn't be changed!
var_dump($template->nodeName);
var_dump($template->innerHTML);

?>
--EXPECT--
string(16) "a<div>foo</div>b"
It is not possible to rename the template element because it hosts a document fragment
string(8) "TEMPLATE"
string(16) "a<div>foo</div>b"
