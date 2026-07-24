--TEST--
Unqualified call to an existing global function does not autoload a namespaced override
--FILE--
<?php
namespace App;

\spl_autoload_register_function_loader(function (string $name) {
    echo "loader($name)\n";
    if ($name === 'App\strlen') {
        eval('namespace App; function strlen(string $s): string { return "shadowed(" . \strlen($s) . ")"; }');
    }
});

function call_unqualified(string $s) {
    return strlen($s);
}

// Global strlen() wins; the loader is not consulted.
var_dump(call_unqualified("hello"));

// A fully qualified call has no global fallback, so it autoloads App\strlen.
var_dump(namespace\strlen("hello"));

// The original call site keeps its cached binding to the global.
var_dump(call_unqualified("hello"));

// A new unqualified call site now resolves to App\strlen.
var_dump(strlen("hello"));
?>
--EXPECT--
int(5)
loader(App\strlen)
string(11) "shadowed(5)"
int(5)
string(11) "shadowed(5)"
