--TEST--
CSS Selectors - Pseudo classes: :lexbor-contains() with an argument longer than its string header
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('<p>needle</p>', LIBXML_NOERROR);

var_dump($dom->querySelectorAll(':lexbor-contains("' . str_repeat('needle', 1024) . '")')->length);
var_dump($dom->querySelectorAll(':lexbor-contains("needle")')->length);

?>
--EXPECT--
int(0)
int(0)
