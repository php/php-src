--TEST--
GH-11290 (DOMElement::replaceWith causes crash)
--EXTENSIONS--
dom
--FILE--
<?php
$html = <<<HTML
<!DOCTYPE HTML>
<html>
<body>
    <p><span class="unwrap_me">Lorem</span><span class="unwrap_me">ipsum</span><span class="unwrap_me">dolor</span></p>
</body>
</html>
HTML;

$dom = new DOMDocument();
$dom->loadHTML($html);

$spans = iterator_to_array($dom->getElementsByTagName('span')->getIterator());
foreach ($spans as $span) {
    if ('unwrap_me' === $span->getAttribute('class')) {
        $span->replaceWith(...$span->childNodes);
    }
}

var_dump(str_replace("\n", "", $dom->saveHTML()));
?>
--EXPECT--
string(67) "<!DOCTYPE HTML><html><body>    <p>Loremipsumdolor</p></body></html>"
