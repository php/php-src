--TEST--
array_map(): foreach optimization resolves the function name of an FCC
--FILE--
<?php

namespace {
    function label($x) { return "global:$x"; }
    function only_global($x) { return "only_global:$x"; }
}

namespace Lib {
    function fmt($x) { return "Lib\\fmt:$x"; }
}

namespace App {
    function label($x) { return "App\\label:$x"; }
    function strtoupper($x) { return "App\\strtoupper:$x"; }
    function pad($x, $y) { return "App\\pad:$x$y"; }

    use function Lib\fmt;
    use function App\label as lbl;

    echo "namespaced function:\n";
    var_dump(\array_map(label(...), [1]));
    echo "namespaced function shadowing an internal one:\n";
    var_dump(\array_map(strtoupper(...), ['a']));
    echo "use function:\n";
    var_dump(\array_map(fmt(...), [2]));
    echo "use function with alias:\n";
    var_dump(\array_map(lbl(...), [3]));
    echo "namespace-relative name:\n";
    var_dump(\array_map(namespace\label(...), [4]));
    echo "fully qualified name:\n";
    var_dump(\array_map(\label(...), [5]));
    echo "fallback to the global function:\n";
    var_dump(\array_map(only_global(...), [6]));
    echo "fallback to an internal function:\n";
    var_dump(\array_map(ucfirst(...), ['b']));
    echo "partial application:\n";
    var_dump(\array_map(pad(?, '!'), [7]));

    if (true) {
        function declared_at_runtime($x) { return "App\\declared_at_runtime:$x"; }
    }
    echo "function declared at runtime:\n";
    var_dump(\array_map(declared_at_runtime(...), [8]));
}

namespace {
    use function Lib\fmt;
    use function Lib\fmt as libfmt;

    echo "global namespace, use function:\n";
    var_dump(array_map(fmt(...), [9]));
    echo "global namespace, use function with alias:\n";
    var_dump(array_map(libfmt(...), [10]));
    echo "global namespace, unqualified:\n";
    var_dump(array_map(label(...), [11]));
}

?>
--EXPECT--
namespaced function:
array(1) {
  [0]=>
  string(11) "App\label:1"
}
namespaced function shadowing an internal one:
array(1) {
  [0]=>
  string(16) "App\strtoupper:a"
}
use function:
array(1) {
  [0]=>
  string(9) "Lib\fmt:2"
}
use function with alias:
array(1) {
  [0]=>
  string(11) "App\label:3"
}
namespace-relative name:
array(1) {
  [0]=>
  string(11) "App\label:4"
}
fully qualified name:
array(1) {
  [0]=>
  string(8) "global:5"
}
fallback to the global function:
array(1) {
  [0]=>
  string(13) "only_global:6"
}
fallback to an internal function:
array(1) {
  [0]=>
  string(1) "B"
}
partial application:
array(1) {
  [0]=>
  string(10) "App\pad:7!"
}
function declared at runtime:
array(1) {
  [0]=>
  string(25) "App\declared_at_runtime:8"
}
global namespace, use function:
array(1) {
  [0]=>
  string(9) "Lib\fmt:9"
}
global namespace, use function with alias:
array(1) {
  [0]=>
  string(10) "Lib\fmt:10"
}
global namespace, unqualified:
array(1) {
  [0]=>
  string(9) "global:11"
}
