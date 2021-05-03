--TEST--
Bug #77686 Removed elements are still returned by getElementById
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadHTML('<html><body id="x"><div id="y"><span id="z"></span></div></body></html>');
$body = $doc->getElementById('x');
$div = $doc->getElementById('y');
$span = $doc->getElementById('z');
$body->removeChild($div);
var_dump($doc->getElementById('y'));
var_dump($doc->getElementById('z'));
?>
--EXPECT--
NULL
NULL
