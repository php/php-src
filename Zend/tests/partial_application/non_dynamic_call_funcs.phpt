--TEST--
Functions that can not be called dynamically, can not be partially applied
--FILE--
<?php

function _func_get_arg() {
    return func_get_arg(?);
}

function _compact() {
    return compact(?);
}

function _extract() {
    return extract(?);
}

function _func_get_args() {
    return func_get_args(?);
}

function _func_num_args() {
    return func_num_args(?);
}

function _get_defined_vars() {
    return get_defined_vars(?);
}

foreach (['_func_get_arg', '_compact', '_extract', '_func_get_args', '_func_num_args', '_get_defined_vars'] as $func) {
    try {
        $func();
        echo "$func\n";
    } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Error: Cannot call func_get_arg() dynamically
Error: Cannot call compact() dynamically
Error: Cannot call extract() dynamically
Error: Cannot call func_get_args() dynamically
Error: Cannot call func_num_args() dynamically
Error: Cannot call get_defined_vars() dynamically
