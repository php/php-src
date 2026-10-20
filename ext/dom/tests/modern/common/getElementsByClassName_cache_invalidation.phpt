--TEST--
getElementsByClassName() cache must be invalidated by class attribute mutations
--EXTENSIONS--
dom
--FILE--
<?php
function mk($body) {
    return Dom\HTMLDocument::createFromString("<!DOCTYPE html><html><body>$body</body></html>");
}

$checks = [
    'className' => function ($doc, $span) { $span->className = 'zzz'; },
    'classList-remove' => function ($doc, $span) { $span->classList->remove('foo'); },
    'classList-value' => function ($doc, $span) { $span->classList->value = 'zzz'; },
    'setAttribute' => function ($doc, $span) { $span->setAttribute('class', 'zzz'); },
    'removeAttribute' => function ($doc, $span) { $span->removeAttribute('class'); },
    'removeAttributeNode' => function ($doc, $span) { $span->removeAttributeNode($span->attributes['class']); },
];
foreach ($checks as $label => $fn) {
    $doc = mk('<span class="foo"></span>');
    $coll = $doc->getElementsByClassName('foo');
    if ($coll->length !== 1) {
        echo "$label: unexpected initial length\n";
        continue;
    }
    $fn($doc, $doc->querySelector('span'));
    echo "$label: ", $coll->length === 0 ? "OK" : "STALE {$coll->length}", "\n";
}

$doc = mk('<span></span>');
$coll = $doc->getElementsByClassName('foo');
var_dump($coll->length);
$doc->querySelector('span')->className = 'foo';
echo $coll->length === 1 ? "growth OK" : "growth STALE", "\n";
?>
--EXPECT--
className: OK
classList-remove: OK
classList-value: OK
setAttribute: OK
removeAttribute: OK
removeAttributeNode: OK
int(0)
growth OK
