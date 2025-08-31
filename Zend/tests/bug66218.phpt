--TEST--
Bug #66218 zend_register_functions breaks reflection
--SKIPIF--
<?php
if (!function_exists("dl")) die("skip need dl");
?>
--FILE--
<?php
$tab = get_extension_funcs("standard");
$fcts = array("dl");
foreach ($fcts as $fct) {
    if (in_array($fct, $tab)) {
        echo "$fct Ok\n";
    }
}
?>
Done
--EXPECT--
dl Ok
Done
