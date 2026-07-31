--TEST--
use function import triggers autoloading of the namespaced function
--FILE--
<?php
namespace App;

use function App\strlen;

\spl_autoload_register_function_loader(function (string $name) {
    echo "loader($name)\n";
    if ($name === 'App\strlen') {
        eval('namespace App; function strlen(string $s): string { return "shadowed(" . \strlen($s) . ")"; }');
    }
});

// The import resolves the call to App\strlen at compile time, so there is
// no global fallback and the loader is consulted.
var_dump(strlen("hello"));
?>
--EXPECT--
loader(App\strlen)
string(11) "shadowed(5)"
