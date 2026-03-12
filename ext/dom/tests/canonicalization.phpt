--TEST--
Test: Canonicalization - C14N()
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<EOXML
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo xmlns="http://www.example.com/ns/foo"
     xmlns:fubar="http://www.example.com/ns/fubar" xmlns:test="urn::test"><contain>
  <bar><test1 /></bar>
  <bar><test2 /></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3 /></fubar:bar>
  <fubar:bar><test4 /></fubar:bar>
<!-- this is a comment -->
</contain>
</foo>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$doc = $dom->documentElement->firstChild;

$newDom = Dom\XMLDocument::createFromString($xml);
$newDoc = $newDom->documentElement->firstChild;
$counter = 0;

function check($doc, $newDoc, ...$args) {
  global $counter;
  $counter++;
  echo $doc->C14N(...$args)."\n\n";
  if ($doc->C14N(...$args) !== $newDoc->C14N(...$args)) {
    var_dump($doc->C14N(...$args), $newDoc->C14N(...$args));
    throw new Error("mismatch: $counter");
  }
}

/* inclusive/without comments first child element of doc element is context. */
check($doc, $newDoc);

/* exclusive/without comments first child element of doc element is context. */
check($doc, $newDoc, TRUE);

/* inclusive/with comments first child element of doc element is context. */
check($doc, $newDoc, FALSE, TRUE);

/* exclusive/with comments first child element of doc element is context. */
check($doc, $newDoc, TRUE, TRUE);

/* exclusive/without comments using xpath query. */
check($doc, $newDoc, TRUE, FALSE, array('query'=>'(//. | //@* | //namespace::*)'))."\n\n";

/* exclusive/without comments first child element of doc element is context.
   using xpath query with registered namespace.
   test namespace prefix is also included. */
check($doc, $newDoc, TRUE, FALSE,
                array('query'=>'(//a:contain | //a:bar | .//namespace::*)',
                      'namespaces'=>array('a'=>'http://www.example.com/ns/foo')),
                array('test'));

/* exclusive/without comments first child element of doc element is context.
   test namespace prefix is also included */
check($doc, $newDoc, TRUE, FALSE, NULL, array('test'));
?>
--EXPECT--
<contain xmlns="http://www.example.com/ns/foo" xmlns:fubar="http://www.example.com/ns/fubar" xmlns:test="urn::test">
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar><test3></test3></fubar:bar>
  <fubar:bar><test4></test4></fubar:bar>

</contain>

<contain xmlns="http://www.example.com/ns/foo">
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3></test3></fubar:bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test4></test4></fubar:bar>

</contain>

<contain xmlns="http://www.example.com/ns/foo" xmlns:fubar="http://www.example.com/ns/fubar" xmlns:test="urn::test">
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar><test3></test3></fubar:bar>
  <fubar:bar><test4></test4></fubar:bar>
<!-- this is a comment -->
</contain>

<contain xmlns="http://www.example.com/ns/foo">
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3></test3></fubar:bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test4></test4></fubar:bar>
<!-- this is a comment -->
</contain>

<foo xmlns="http://www.example.com/ns/foo"><contain>
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3></test3></fubar:bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test4></test4></fubar:bar>

</contain>
</foo>

<contain xmlns="http://www.example.com/ns/foo" xmlns:test="urn::test"><bar></bar><bar></bar></contain>

<contain xmlns="http://www.example.com/ns/foo" xmlns:test="urn::test">
  <bar><test1></test1></bar>
  <bar><test2></test2></bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test3></test3></fubar:bar>
  <fubar:bar xmlns:fubar="http://www.example.com/ns/fubar"><test4></test4></fubar:bar>

</contain>
