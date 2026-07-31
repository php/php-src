--TEST--
JIT INIT_FCALL_BY_NAME: function autoloading preserves the original case
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
spl_autoload_register_function_loader(function (string $name) {
    echo "loader($name)\n";
    if (strtolower($name) === 'jit_func') {
        eval('function jit_func() { return "jit_ok"; }');
    }
});

function test() {
    return Jit_Func();
}
var_dump(test());
var_dump(test());
?>
--EXPECT--
loader(Jit_Func)
string(6) "jit_ok"
string(6) "jit_ok"
