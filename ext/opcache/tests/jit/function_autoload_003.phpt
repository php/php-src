--TEST--
JIT INIT_NS_FCALL_BY_NAME: function autoloading with namespace fallback
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
opcache.jit_buffer_size=16M
--EXTENSIONS--
opcache
--FILE--
<?php
namespace App;

\spl_autoload_register_function_loader(function (string $name) {
    echo "Autoloading: $name\n";
    if ($name === 'App\ns_func') {
        eval('namespace App; function ns_func() { return "ns_ok"; }');
    }
});

function test() {
    return ns_func();
}
var_dump(test());
var_dump(test());
?>
--EXPECT--
Autoloading: App\ns_func
string(5) "ns_ok"
string(5) "ns_ok"
