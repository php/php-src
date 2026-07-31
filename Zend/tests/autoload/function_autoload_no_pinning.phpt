--TEST--
Function autoloading does not pin lookup results (no observable leak into function_exists)
--FILE--
<?php
// Function autoloading does not pin lookup results. The 2023
// core-autoloading RFC pinned a namespaced lookup to the global function it
// fell back to, making function_exists('Foo\strlen') return true after an
// unqualified strlen() call inside namespace Foo -- a leak the RFC flagged as
// "a large BC break". Here nothing enters the function table except functions a
// loader actually defines, so function_exists() stays false and Foo\strlen
// stays autoloadable.

namespace {
    $consulted = 0;
    spl_autoload_register_function_loader(function (string $name) use (&$consulted) {
        $consulted++;
        echo "loader($name)\n";
        if ($name === 'Foo\strlen') {
            eval('namespace Foo; function strlen($s) { return "foo-strlen"; }');
        }
    });
}

namespace Bar {
    // Before any call in Foo: the namespaced name does not exist.
    // Probe without autoloading so we observe the table, not trigger a load.
    var_dump(function_exists('Foo\strlen', false));
}

namespace Foo {
    // Unqualified call: namespaced miss, global strlen hits, fallback wins.
    // The global fallback short-circuits before the loader is reached.
    var_dump(strlen('hello'));
}

namespace Bar {
    // No pinning: the fallback did not make Foo\strlen "exist".
    var_dump(function_exists('Foo\strlen', false));
}

namespace Foo {
    // And because nothing was pinned, a fully qualified call can still
    // autoload the namespaced version on demand.
    var_dump(namespace\strlen('hello'));
    var_dump(function_exists('Foo\strlen', false));
}

namespace {
    echo "loader consulted: $consulted\n";
}
?>
--EXPECT--
bool(false)
int(5)
bool(false)
loader(Foo\strlen)
string(10) "foo-strlen"
bool(true)
loader consulted: 1
