--TEST--
GH-11289 (DOMException: Not Found Error introduced in 8.2.6, 8.1.19)
--EXTENSIONS--
dom
--FILE--
<?php

$html = <<<HTML
<!DOCTYPE HTML>
<html>
<body>
    <div></div>
</body>
</html>
HTML;

$dom = new DOMDocument();
$dom->loadHTML($html);

$divs = iterator_to_array($dom->getElementsByTagName('div')->getIterator());
foreach ($divs as $div) {
    $fragment = $dom->createDocumentFragment();
    $fragment->appendXML('<p>Hi!</p>');
    $div->replaceWith(...$fragment->childNodes);
}

var_dump(str_replace("\n", "", $dom->saveHTML()));
?>
--EXPECT--
string(55) "<!DOCTYPE HTML><html><body>    <p>Hi!</p></body></html>"
