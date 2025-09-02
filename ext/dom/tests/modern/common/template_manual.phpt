--TEST--
<template> element manual creation
--EXTENSIONS--
dom
--FILE--
<?php

echo "=== After creation ===\n";

$dom = Dom\HTMLDocument::createEmpty();
$template = $dom->appendChild($dom->createElement("template"));
var_dump($template->innerHTML);
echo $dom->saveXML(), "\n";
echo $dom->saveHTML(), "\n";

echo "=== After setting content ===\n";

$template->innerHTML = "<p>hello</template></p>";
var_dump($template->innerHTML);
var_dump($template->firstChild);
echo $dom->saveXML(), "\n";
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
=== After creation ===
string(0) ""
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<template xmlns="http://www.w3.org/1999/xhtml"></template>
<template></template>
=== After setting content ===
string(12) "<p>hello</p>"
NULL
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<template xmlns="http://www.w3.org/1999/xhtml"><p>hello</p></template>
<template><p>hello</p></template>
