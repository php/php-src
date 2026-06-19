--TEST--
declare(strict_namespace=1) disables global fallback for unqualified constants
--FILE--
<?php
declare(strict_namespace=1);

namespace Foo;

const GREETING = 'foo-greeting';        // Foo\GREETING
\define('BAREWORD', 'global-bareword'); // global constant

// unqualified: resolves to Foo\GREETING
\var_dump(GREETING);

// fully qualified: global constant
\var_dump(\BAREWORD);

// unqualified BAREWORD resolves to Foo\BAREWORD (undefined), no fallback
try {
    \var_dump(BAREWORD);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

// PHP_INT_MAX resolves to Foo\PHP_INT_MAX; fully qualified reaches the global
try {
    \var_dump(PHP_INT_MAX);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
\var_dump(\is_int(\PHP_INT_MAX));

// constant expressions follow the same rule
const PREFIX = 'p-';
const COMBINED = PREFIX . 'x';
\var_dump(COMBINED);

// true/false/null are keywords, never namespace-resolved
\var_dump(null, true, false);
?>
--EXPECT--
string(12) "foo-greeting"
string(15) "global-bareword"
Undefined constant "Foo\BAREWORD"
Undefined constant "Foo\PHP_INT_MAX"
bool(true)
string(3) "p-x"
NULL
bool(true)
bool(false)
