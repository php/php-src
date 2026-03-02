--TEST--
Template cloning
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createFromString('<template>x</template>', LIBXML_NOERROR);
$a = $dom->head->firstChild->cloneNode(false);
echo $dom->saveXML($a), "\n";
echo $dom->saveHTML($a), "\n";
?>
--EXPECT--
<template xmlns="http://www.w3.org/1999/xhtml"></template>
<template></template>
