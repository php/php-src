--TEST--
use-import aliases are matched case-sensitively
--FILE--
<?php
namespace App {
    class Widget {}
    function helper(): string { return 'helper'; }
}

namespace {
    use App\Widget;
    use function App\helper;

    var_dump(get_class(new Widget()));
    var_dump(helper());

    // Wrong-case alias does not resolve through the import; it refers to a
    // (nonexistent) global symbol instead.
    try {
        new widget();
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        HELPER();
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
string(10) "App\Widget"
string(6) "helper"
Class "widget" not found
Call to undefined function HELPER()
