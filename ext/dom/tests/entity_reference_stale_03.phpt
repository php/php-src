--TEST--
Entity references with stale entity declaration 03
--EXTENSIONS--
dom
--FILE--
<?php

foreach (['firstChild', 'lastChild', 'textContent', 'childNodes'] as $prop) {
    $dom = Dom\XMLDocument::createFromString(<<<XML
    <!DOCTYPE foo [
    <!ENTITY foo "bar">
    ]>
    <foo>&foo;</foo>
    XML);

    $ref = $dom->documentElement->firstChild;
    $decl = $ref->firstChild;

    $nodes = $ref->childNodes;
    $dom->removeChild($dom->doctype);
    unset($decl);

    var_dump($ref->$prop);
}

?>
--EXPECT--
NULL
NULL
NULL
object(Dom\NodeList)#1 (1) {
  ["length"]=>
  int(0)
}
