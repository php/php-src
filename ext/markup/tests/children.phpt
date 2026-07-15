--TEST--
Markup\Element children: scalar coercion, recursive array flattening, Traversables, and the nesting-depth bound
--EXTENSIONS--
markup
--FILE--
<?php
use Markup\Element;
use Markup\Element as E;
use function Markup\raw;

// --- scalar and array child coercion ---

// Scalars: int/float stringify, true -> "1", false/null -> "".
echo (new Element('p', [], [1, ' ', 2.5, ' ', true, false, null]))->__toString(), "\n";

// Arrays flatten recursively (the array_map loop idiom).
$items = ['Ada', 'Linus', 'Grace'];
echo (new Element('ul', [], [
    array_map(fn($n) => new Element('li', [], [$n]), $items),
]))->__toString(), "\n";

// A nested Html passes through un-escaped.
echo (new Element('div', [], [raw('<b>bold</b>')]))->__toString(), "\n";

// Nested elements are Html too.
echo (new Element('div', [], [new Element('em', [], ['hi & bye'])]))->__toString(), "\n";

// Non-Stringable object is a hard error.
try {
    (new Element('div', [], [new stdClass]))->__toString();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// --- Traversable children: generators, Iterator, IteratorAggregate ---

// Generator
function rows(): Generator {
    yield new Element('li', [], ['one']);
    yield new Element('li', [], ['two']);
}
echo (new Element('ul', [], [rows()]))->__toString(), "\n";

// ArrayIterator (Iterator), mixing scalars and Html, with keys ignored
$it = new ArrayIterator(['a' => 'x & y', 'b' => new Element('b', [], ['z'])]);
echo (new Element('p', [], [$it]))->__toString(), "\n";

// IteratorAggregate, nested inside an array (recursive flattening)
$agg = new class implements IteratorAggregate {
    public function getIterator(): Iterator {
        return new ArrayIterator([new Element('span', [], ['nested'])]);
    }
};
echo (new Element('div', [], [['before ', $agg, ' after']]))->__toString(), "\n";

// An exception thrown mid-iteration propagates.
function boom(): Generator {
    yield new Element('i', [], ['ok']);
    throw new RuntimeException('stop');
}
try {
    (new Element('div', [], [boom()]))->__toString();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// --- nesting-depth bound ---

// A cheap-to-build, deeply nested array must throw, not crash the process.
$kids = ['x'];
for ($i = 0; $i < 2048; $i++) {
    $kids = [$kids];
}
try {
    echo (new E('div', [], $kids))->__toString(), "\n";
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Normal (shallow) nesting still renders.
echo (new E('ul', [], [[[new E('li', [], ['ok'])]]]))->__toString(), "\n";
?>
--EXPECTF--
<p>1 2.5 1</p>
<ul><li>Ada</li><li>Linus</li><li>Grace</li></ul>
<div><b>bold</b></div>
<div><em>hi &amp; bye</em></div>
TypeError: Object of class stdClass cannot be rendered as a markup child: it implements none of Markup\Html, Stringable, or Traversable
<ul><li>one</li><li>two</li></ul>
<p>x &amp; y<b>z</b></p>
<div>before <span>nested</span> after</div>
RuntimeException: stop
Error: Maximum markup nesting depth of 1024 exceeded
<ul><li>ok</li></ul>
