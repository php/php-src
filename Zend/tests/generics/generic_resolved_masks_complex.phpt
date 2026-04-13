--TEST--
Generic class: resolved_masks slow path for complex types (generic class refs, nested)
--DESCRIPTION--
Tests that the resolved_masks optimization correctly falls through to the
slow path for complex types like generic class references (Box<int>),
class types, and nested generics. The mask is 0 for these, forcing
the full type check.
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Generic class ref as type arg: Box<Box<int>>
// resolved_masks[0] should be 0 (complex type), falling through to slow path
$inner = new Box<int>(42);
$outer = new Box<Box<int>>($inner);
echo "nested create: OK\n";

// Method return with nested generic
echo $outer->get()->get() . "\n";

// Property access on nested generic
echo $outer->value->value . "\n";

// Constructor type check (slow path: verifies Box<int> arg matches T=Box<int>)
try {
    $bad = new Box<Box<int>>("not a box");
} catch (TypeError $e) {
    echo "nested ctor reject: OK\n";
}

// Wrong inner type
$wrongInner = new Box<string>("hello");
try {
    $bad2 = new Box<Box<int>>($wrongInner);
} catch (TypeError $e) {
    echo "wrong inner type reject: OK\n";
}

// Property assignment on nested generic (triggers slow path type check)
try {
    $outer->get()->value = "not an int";
} catch (TypeError $e) {
    echo "nested prop reject: OK\n";
}

// Valid property assignment on inner
$outer->get()->value = 100;
echo $outer->get()->value . "\n";

// Class type arg (non-generic class)
interface Printable {
    public function display(): string;
}

class Label implements Printable {
    public function __construct(private string $text) {}
    public function display(): string { return $this->text; }
}

class Wrapper<T: Printable> {
    public function __construct(private T $item) {}
    public function show(): string { return $this->item->display(); }
}

$w = new Wrapper<Label>(new Label("hello"));
echo $w->show() . "\n";

// Triple nesting: Box<Box<Box<int>>>
$deep = new Box<Box<Box<int>>>(new Box<Box<int>>(new Box<int>(7)));
echo $deep->get()->get()->get() . "\n";

echo "Done.\n";
?>
--EXPECT--
nested create: OK
42
42
nested ctor reject: OK
wrong inner type reject: OK
nested prop reject: OK
100
hello
7
Done.
