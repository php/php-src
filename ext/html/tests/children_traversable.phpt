--TEST--
Html\Element flattens Traversable children: generators, Iterator, IteratorAggregate (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element;

// Generator
function rows(): Generator {
    yield new Element('li', [], ['one']);
    yield new Element('li', [], ['two']);
}
echo (new Element('ul', [], [rows()]))->__toString(), "\n";

// ArrayIterator (Iterator), mixing scalars and Htmlable, with keys ignored
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
    echo get_class($e), ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
<ul><li>one</li><li>two</li></ul>
<p>x &amp; y<b>z</b></p>
<div>before <span>nested</span> after</div>
RuntimeException: stop
