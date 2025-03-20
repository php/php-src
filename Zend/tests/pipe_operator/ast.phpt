--TEST--
A pipe operator displays as a pipe operator when outputting syntax, with correct parens.
--FILE--
<?php

print "Concat, which binds higher\n";

try {
    assert(false && foo() . bar() |> baz() . quux());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() . bar()) |> baz() . quux());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . (bar() |> baz()) . quux());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . bar() |> (baz() . quux()));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() . bar() |> baz()) . quux());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . (bar() |> baz() . quux()));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

print "<, which binds lower\n";

try {
    assert(false && foo() < bar() |> baz());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() < bar()) |> baz());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() < (bar() |> baz()));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() |> bar() < baz());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() |> bar()) < baz());
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() |> (bar() < baz()));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}



print "misc examples\n";

try {
    assert(false && foo() |> (bar() |> baz(...)));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Concat, which binds higher
assert(false && foo() . bar() |> baz() . quux())
assert(false && foo() . bar() |> baz() . quux())
assert(false && foo() . (bar() |> baz()) . quux())
assert(false && foo() . bar() |> baz() . quux())
assert(false && (foo() . bar() |> baz()) . quux())
assert(false && foo() . (bar() |> baz() . quux()))
<, which binds lower
assert(false && foo() < bar() |> baz())
assert(false && (foo() < bar()) |> baz())
assert(false && foo() < bar() |> baz())
assert(false && foo() |> bar() < baz())
assert(false && foo() |> bar() < baz())
assert(false && foo() |> (bar() < baz()))
misc examples
assert(false && foo() |> (bar() |> baz(...)))
