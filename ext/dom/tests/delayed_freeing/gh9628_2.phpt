--TEST--
GH-9628 (Implicitly removing nodes from \DOMDocument breaks existing references) - advanced variation
--EXTENSIONS--
dom
--FILE--
<?php
$html = <<<'HTML'
<p>
    <span>
        <strong>
            <span>
                Test
            </span>
        </strong>
    </span>
</p>
HTML;

$doc = new \DOMDocument('1.0', 'UTF-8');
$doc->loadHTML(
    '<html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" /></head><body>' . $html . '</body></html>'
);
$xpath = new \DOMXPath($doc);

$spans = [];
foreach ($xpath->query('//span') as $span) {
    $spans[] = $span;
}

\assert(\count($spans) === 2);

foreach ($spans as $span) {
    \assert($span instanceof \DOMElement);
    \assert($span->ownerDocument === $doc);
}

foreach ($spans as $span) {
    \assert($span instanceof \DOMElement);

    // This call will fail for the second `<span>`. It does not really
    // matter what is accessed here, the error message will be the same.
    $span->hasAttribute('test');

    while ($span->childNodes->length) {
        $span->removeChild($span->childNodes[0]);
    }
    $span->appendChild(
        $span->ownerDocument->createTextNode('Hello World')
    );
}

echo $spans[0]->textContent . "\n"; // Hello World
?>
--EXPECT--
Hello World
