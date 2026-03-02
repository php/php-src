--TEST--
GH-15570 (Segmentation fault (access null pointer) in ext/dom/html5_serializer.c)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
dom
--FILE--
<?php
$html = <<<HTML
<head>
</html>
HTML;
$dom = Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
$a = $dom->head->firstChild->cloneNode(false);
var_dump($dom->saveHTML($a));
?>
--EXPECT--
string(1) "
"
