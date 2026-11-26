--TEST--
GH-25639: UAF in report_class_fetch_error() when the autoloader frees the class name
--CREDITS--
dhg0x
--FILE--
<?php

spl_autoload_register(function ($cls) {
    $GLOBALS['cls'] = null;
});

function test() {
    global $cls;
    $cls::method();
}

$cls = implode("", ["Undefined", "Class"]);
try {
    test();
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class "UndefinedClass" not found
