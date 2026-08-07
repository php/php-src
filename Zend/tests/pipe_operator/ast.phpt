--TEST--
A pipe operator displays as a pipe operator when outputting syntax, with correct parens.
--FILE--
<?php

print "Concat, which binds higher\n";

try {
    assert(false && foo() . bar() |> baz() . quux());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() . bar()) |> baz() . quux());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . (bar() |> baz()) . quux());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . bar() |> (baz() . quux()));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() . bar() |> baz()) . quux());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() . (bar() |> baz() . quux()));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

print "<, which binds lower\n";

try {
    assert(false && foo() < bar() |> baz());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() < bar()) |> baz());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() < (bar() |> baz()));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() |> bar() < baz());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && (foo() |> bar()) < baz());
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    assert(false && foo() |> (bar() < baz()));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}



print "misc examples\n";

try {
    assert(false && foo() |> (bar() |> baz(...)));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Concat, which binds higher
AssertionError: assert(false && foo() . bar() |> baz() . quux())
AssertionError: assert(false && foo() . bar() |> baz() . quux())
AssertionError: assert(false && foo() . (bar() |> baz()) . quux())
AssertionError: assert(false && foo() . bar() |> baz() . quux())
AssertionError: assert(false && (foo() . bar() |> baz()) . quux())
AssertionError: assert(false && foo() . (bar() |> baz() . quux()))
<, which binds lower
AssertionError: assert(false && foo() < bar() |> baz())
AssertionError: assert(false && (foo() < bar()) |> baz())
AssertionError: assert(false && foo() < bar() |> baz())
AssertionError: assert(false && foo() |> bar() < baz())
AssertionError: assert(false && foo() |> bar() < baz())
AssertionError: assert(false && foo() |> (bar() < baz()))
misc examples
AssertionError: assert(false && foo() |> (bar() |> baz(...)))
