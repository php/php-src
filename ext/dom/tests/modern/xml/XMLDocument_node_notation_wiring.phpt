--TEST--
Dom\XMLDocument: Dom\Notation nodes are connected to their document and doctype
--EXTENSIONS--
dom
--FILE--
<?php
$cases = [
    'GIF' => '<!NOTATION GIF SYSTEM "image/gif">',
    'JPEG' => '<!NOTATION JPEG PUBLIC "-//W3C//NOTATION JPEG//EN" "image/jpeg">',
    'HTML' => '<!NOTATION HTML PUBLIC "-//W3C//NOTATION HTML//EN">',
];

foreach ($cases as $name => $declaration) {
    $xml = <<<XML
<!DOCTYPE root [
    $declaration
]>
<root/>
XML;

    $dom = Dom\XMLDocument::createFromString($xml);
    $doctype = $dom->doctype;
    $notations = $doctype->notations;

    echo "=== $name ===\n";

    $namedNotation = $notations->getNamedItem($name);
    foreach ($notations as $iteratedNotation) {
        // getNamedItem
        var_dump($namedNotation->nodeName);
        var_dump($namedNotation->textContent);
        var_dump($namedNotation->nodeValue);
        var_dump($namedNotation->isConnected);
        var_dump($namedNotation->ownerDocument === $dom);
        var_dump($namedNotation->parentNode === $doctype);
        var_dump($namedNotation->parentElement);

        // iteration
        var_dump($iteratedNotation->nodeName);
        var_dump($iteratedNotation->textContent);
        var_dump($iteratedNotation->nodeValue);
        var_dump($iteratedNotation->isConnected);
        var_dump($iteratedNotation->ownerDocument === $dom);
        var_dump($iteratedNotation->parentNode === $doctype);
        var_dump($iteratedNotation->parentElement);

        // wiring
        // getNamedItem and iteration each allocate a fresh Notation instance                                                                                                 
        var_dump($namedNotation !== $iteratedNotation);
    }
}
?>
--EXPECT--
=== GIF ===
string(3) "GIF"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
string(3) "GIF"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
bool(true)
=== JPEG ===
string(4) "JPEG"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
string(4) "JPEG"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
bool(true)
=== HTML ===
string(4) "HTML"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
string(4) "HTML"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
bool(true)
