--TEST--
Bug #69679 (DOMDocument::loadHTML refuses to accept NULL bytes)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$html = "<!DOCTYPE html><html><head><meta charset='UTF-8'></head><body>U+0000 <span>\x0</span></body></html>";
$doc->loadHTML($html);
print($doc->saveHTML());
?>
--EXPECT--
<!DOCTYPE html>
<html><head><meta charset="UTF-8"></head><body>U+0000 <span></span></body></html>
