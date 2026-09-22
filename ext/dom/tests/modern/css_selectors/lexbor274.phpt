--TEST--
Lexbor #274
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString('<html><body><div></div>\n</body></html>', LIBXML_NOERROR);
var_dump($dom->querySelector('a:has(+ b)'));

?>
--EXPECT--
NULL
