--TEST--
A pipe operator displays as a pipe operator when outputting syntax, with correct parens.
--FILE--
<?php
/*
function _test(int $a): int {
    return $a + 1;
}

function abool(int $x): bool {
    return false;
}

try {
    assert((5 |> '_test') == 99);
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert((5 |> _test(...)) == 99);
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(5 |> abool(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}
*/

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
