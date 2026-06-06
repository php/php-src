--TEST--
Namespaced function autoloading
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    echo "Autoloading: $name\n";
    if ($name === 'App\Util\helper') {
        eval('namespace App\Util; function helper() { return "ns_helper"; }');
    }
});

var_dump(\App\Util\helper());
?>
--EXPECT--
Autoloading: App\Util\helper
string(9) "ns_helper"
