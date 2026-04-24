--TEST--
Dom\XMLDocument: Dom\Notation nodes are connected to their document and doctype
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<XML
<!DOCTYPE root [
    <!NOTATION GIF SYSTEM "image/gif">
    <!NOTATION JPEG PUBLIC "-//W3C//NOTATION JPEG//EN" "image/jpeg">
    <!NOTATION HTML PUBLIC "-//W3C//NOTATION HTML//EN">
]>
<root/>
XML;

$dom = Dom\XMLDocument::createFromString($xml);
$doctype = $dom->doctype;
$notations = $doctype->notations;

// Make notations deterministic by name, as the order of notations in the map is not guaranteed
foreach (['GIF', 'JPEG', 'HTML'] as $name) {
    echo "=== $name ===\n";
    $notation = $notations->getNamedItem($name);
    var_dump($notation->nodeName);
    var_dump($notation->textContent);
    var_dump($notation->nodeValue);
    var_dump($notation->isConnected);
    var_dump($notation->ownerDocument === $dom);
    var_dump($notation->parentNode === $doctype);
    var_dump($notation->parentElement);
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
=== JPEG ===
string(4) "JPEG"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
=== HTML ===
string(4) "HTML"
NULL
NULL
bool(true)
bool(true)
bool(true)
NULL
