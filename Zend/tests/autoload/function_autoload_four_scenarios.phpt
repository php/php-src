--TEST--
Function autoloading: the four namespace resolution scenarios for an unqualified call
--FILE--
<?php
namespace App;

$count = 0;
\spl_autoload_register_function_loader(function (string $name) use (&$count) {
    $count++;
    echo "loader($name)\n";
    if ($name === 'App\s3') {
        eval('namespace App; function s3() { return "ns s3"; }');
    } elseif ($name === 'App\s4') {
        // Answer a request for App\s4 by defining the GLOBAL function instead.
        eval('function s4() { return "global s4"; }');
    }
});

// 1. App\s1 is already defined: it is called, loader not consulted.
function s1() { return "ns s1"; }
\var_dump(s1());

// 2. Only global s2 is defined: global fallback is used, loader not consulted.
eval('function s2() { return "global s2"; }');
\var_dump(s2());

// 3. Neither defined: loader is called once with App\s3 and defines it.
\var_dump(s3());
\var_dump(s3()); // already defined: no second consult

// 4. Neither defined, loader defines global s4 instead of App\s4: the current
//    call still throws (an answer to a different question is not rebound),
//    then the next call finds the global through the normal fallback.
try {
    \var_dump(s4());
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
\var_dump(s4()); // global found via fallback: no re-consult

echo "loader calls: $count\n";
\var_dump(\function_exists('App\s4', false)); // no pinning: never became App\s4
?>
--EXPECT--
string(5) "ns s1"
string(9) "global s2"
loader(App\s3)
string(5) "ns s3"
string(5) "ns s3"
loader(App\s4)
Call to undefined function App\s4()
string(9) "global s4"
loader calls: 2
bool(false)
