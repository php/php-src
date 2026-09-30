--TEST--
tidyNode objects are invalid after reparsing their document
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = tidy_parse_string('<html><body><p>one</p><p>two</p></body></html>');
$node = $tidy->body()->child[0];
var_dump($node->isHtml());
var_dump($node->hasSiblings());

$tidy->parseString('<html><body><p>three</p></body></html>');

$operations = [
    'string cast' => static fn() => (string) $node,
    'hasChildren' => static fn() => $node->hasChildren(),
    'hasSiblings' => static fn() => $node->hasSiblings(),
    'isComment' => static fn() => $node->isComment(),
    'isHtml' => static fn() => $node->isHtml(),
    'isText' => static fn() => $node->isText(),
    'isJste' => static fn() => $node->isJste(),
    'isAsp' => static fn() => $node->isAsp(),
    'isPhp' => static fn() => $node->isPhp(),
    'getParent' => static fn() => $node->getParent(),
    'getPreviousSibling' => static fn() => $node->getPreviousSibling(),
    'getNextSibling' => static fn() => $node->getNextSibling(),
];

foreach ($operations as $operation => $callback) {
    try {
        $callback();
        echo $operation, ": no error\n";
    } catch (Error $e) {
        echo $operation, ': ', $e::class, ': ', $e->getMessage(), "\n";
    }
}

var_dump($tidy->body()->child[0]->isHtml());

?>
--EXPECT--
bool(true)
bool(true)
string cast: Error: tidyNode object is no longer valid after its document was reparsed
hasChildren: Error: tidyNode object is no longer valid after its document was reparsed
hasSiblings: Error: tidyNode object is no longer valid after its document was reparsed
isComment: Error: tidyNode object is no longer valid after its document was reparsed
isHtml: Error: tidyNode object is no longer valid after its document was reparsed
isText: Error: tidyNode object is no longer valid after its document was reparsed
isJste: Error: tidyNode object is no longer valid after its document was reparsed
isAsp: Error: tidyNode object is no longer valid after its document was reparsed
isPhp: Error: tidyNode object is no longer valid after its document was reparsed
getParent: Error: tidyNode object is no longer valid after its document was reparsed
getPreviousSibling: Error: tidyNode object is no longer valid after its document was reparsed
getNextSibling: Error: tidyNode object is no longer valid after its document was reparsed
bool(true)
