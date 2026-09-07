--TEST--
GH-22522 (Double-free of namespaces when re-parsing while iterating in-scope namespaces)
--CREDITS--
Yuancheng Jiang (YuanchengJiang)
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<<XML
<root xmlns="urn:a">
    <child xmlns="">
        <c:child xmlns:c="urn:c"/>
    </child>
    <b:sibling xmlns:b="urn:b" xmlns:d="urn:d" d:foo="bar">
        <d:child xmlns:d="urn:d2"/>
    </b:sibling>
</root>
XML;

function dump($dom, $name, $depth) {
    $list = $dom->getElementsByTagName($name)[0]->getInScopeNamespaces();
    foreach ($list as $entry) {
        // Re-parse namespace-heavy markup while a snapshot of the in-scope
        // namespaces is still alive; used to double-free on teardown.
        $reparsed = Dom\XMLDocument::createFromString($GLOBALS['xml']);
        if ($depth > 0) {
            dump($reparsed, 'c:child', $depth - 1);
            dump($reparsed, 'child', $depth - 1);
        }
        echo $entry->prefix, " => ", $entry->namespaceURI, "\n";
    }
}

$dom = Dom\XMLDocument::createFromString($xml);
dump($dom, 'c:child', 3);
dump($dom, 'child', 3);
echo "done\n";

?>
--EXPECT--
c => urn:c
c => urn:c
c => urn:c
c => urn:c
done
