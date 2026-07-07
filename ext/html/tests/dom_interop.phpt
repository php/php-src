--TEST--
DOM interop: Element::toDom() and accepting a DOM node as a child (RFC §8)
--EXTENSIONS--
html
dom
--FILE--
<?php
use Html\Element as E;

$doc = Dom\HTMLDocument::createEmpty();

// (1) toDom: markup -> real DOM nodes owned by $doc
$node = (<div class="card"><h1>Hi & bye</h1><p>Body <em>x</em></p></div>)->toDom($doc);
echo get_class($node), "\n";
echo $doc->saveHtml($node), "\n";
echo $node->firstChild->firstChild->textContent, "\n"; // queryable DOM tree

// (2) a modern Dom\Node accepted as a markup child (passes through as its HTML)
$d2 = Dom\HTMLDocument::createFromString('<!DOCTYPE html><body><span id="s">from &amp; dom</span>');
$span = $d2->getElementById('s');
echo (<section>{$span}</section>), "\n";

// (3) a legacy DOMNode also works (saveHTML, case-insensitive)
$leg = new DOMDocument();
$leg->loadHTML('<p>legacy &amp; node</p>', LIBXML_NOERROR);
$p = $leg->getElementsByTagName('p')->item(0);
echo (<div>{$p}</div>), "\n";

// (4) round-trip: markup -> DOM -> back into markup
$frag = (<ul><li>a</li><li>b</li></ul>)->toDom($doc);
echo (<div>{$frag}</div>), "\n";

// (5) toDom requires a Dom\Document
try {
    (<p/>)->toDom(new stdClass);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Dom\DocumentFragment
<div class="card"><h1>Hi &amp; bye</h1><p>Body <em>x</em></p></div>
Hi & bye
<section><span id="s">from &amp; dom</span></section>
<div><p>legacy &amp; node</p></div>
<div><ul><li>a</li><li>b</li></ul></div>
Html\Element::toDom(): Argument #1 ($document) must be of type ?Dom\Document, stdClass given
