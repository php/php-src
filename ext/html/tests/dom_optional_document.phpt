--TEST--
DOM interop: toDom() creates an empty HTMLDocument when no document is given
--EXTENSIONS--
html
dom
--FILE--
<?php
use Html\Element as E;
use Html\Fragment;
use Html\Raw;

// (1) Element::toDom() with no argument: fragment owned by a fresh document
$frag = (<div class="card"><h1>Hi & bye</h1></div>)->toDom();
echo get_class($frag), "\n";
echo get_class($frag->ownerDocument), "\n";
echo $frag->ownerDocument->saveHtml($frag), "\n";
echo $frag->firstChild->firstChild->textContent, "\n"; // queryable DOM tree

// (2) explicit null behaves the same as omitting the argument
$frag = (<p>null doc</p>)->toDom(null);
echo $frag->ownerDocument->saveHtml($frag), "\n";

// (3) the owner document stays alive through the fragment alone
$li = (<ul><li>a</li><li>b</li></ul>)->toDom()->firstChild->firstChild;
gc_collect_cycles();
echo $li->textContent, "\n";

// (4) Fragment and Raw get the same default
echo (<>one<b>two</b></>)->toDom()->childNodes->length, "\n";
$raw = (new Raw('<i>raw</i>'))->toDom();
echo $raw->ownerDocument->saveHtml($raw), "\n";

// (5) each call gets its own document
$a = (<p/>)->toDom();
$b = (<p/>)->toDom();
var_dump($a->ownerDocument === $b->ownerDocument);

// (6) passing a document still targets that document
$doc = Dom\HTMLDocument::createEmpty();
var_dump((<p/>)->toDom($doc)->ownerDocument === $doc);
?>
--EXPECT--
Dom\DocumentFragment
Dom\HTMLDocument
<div class="card"><h1>Hi &amp; bye</h1></div>
Hi & bye
<p>null doc</p>
a
2
<i>raw</i>
bool(false)
bool(true)
