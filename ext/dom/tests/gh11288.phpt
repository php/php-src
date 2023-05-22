--TEST--
GH-11288 (Error: Couldn't fetch DOMElement introduced in 8.2.6, 8.1.19)
--FILE--
<?php
$html = <<<HTML
<!DOCTYPE HTML>
<html>
<span class="unwrap_me"><i><span class="foo">Lorem</span></i><span class="foo">ipsum</span></span>
</html>
HTML;

$dom = new DOMDocument();
$dom->loadHTML($html);

$spans = iterator_to_array($dom->getElementsByTagName('span')->getIterator());
foreach ($spans as $span) {
    if ('unwrap_me' === $span->getAttribute('class')) {
        $fragment = $dom->createDocumentFragment();
        $fragment->append(...$span->childNodes);
        $span->parentNode?->replaceChild($fragment, $span);
    }
}

var_dump($dom->saveHTML());

$html = <<<HTML
<!DOCTYPE HTML>
<html>
<span class="unwrap_me"><i><span class="foo">Lorem</span></i><span class="foo">ipsum</span></span>
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

var_dump($dom->saveHTML());

$html = <<<HTML
<!DOCTYPE HTML>
<html>
<span class="unwrap_me"><i><span class="foo">Lorem</span></i><span class="foo">ipsum</span></span>
</html>
HTML;

$dom = new DOMDocument();
$dom->loadHTML($html);

$spans = iterator_to_array($dom->getElementsByTagName('span')->getIterator());
foreach ($spans as $span) {
    if ('unwrap_me' === $span->getAttribute('class')) {
        $span->replaceWith('abc');
    }
}

var_dump($dom->saveHTML());
?>
--EXPECT--
string(112) "<!DOCTYPE HTML>
<html>
<body><i><span class="foo">Lorem</span></i><span class="foo">ipsum</span>
</body></html>
"
string(112) "<!DOCTYPE HTML>
<html>
<body><i><span class="foo">Lorem</span></i><span class="foo">ipsum</span>
</body></html>
"
string(48) "<!DOCTYPE HTML>
<html>
<body>abc
</body></html>
"
