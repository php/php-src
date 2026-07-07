--TEST--
Html\Element child coercion: scalars, arrays flatten, Htmlable passes through (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;
use function Html\raw;

// Scalars: int/float stringify, true -> "1", false/null -> "".
echo (new Element('p', [], [1, ' ', 2.5, ' ', true, false, null]))->__toString(), "\n";

// Arrays flatten recursively (the array_map loop idiom).
$items = ['Ada', 'Linus', 'Grace'];
echo (new Element('ul', [], [
    array_map(fn($n) => new Element('li', [], [$n]), $items),
]))->__toString(), "\n";

// A nested Htmlable passes through un-escaped.
echo (new Element('div', [], [raw('<b>bold</b>')]))->__toString(), "\n";

// Nested elements are Htmlable too.
echo (new Element('div', [], [new Element('em', [], ['hi & bye'])]))->__toString(), "\n";

// Non-Stringable object is a hard error.
try {
    (new Element('div', [], [new stdClass]))->__toString();
} catch (\Throwable $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
<p>1 2.5 1</p>
<ul><li>Ada</li><li>Linus</li><li>Grace</li></ul>
<div><b>bold</b></div>
<div><em>hi &amp; bye</em></div>
TypeError: Object of class stdClass cannot be rendered as a markup child: it implements none of Html\Htmlable, Stringable, or Traversable, and is not a DOM node
