<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('libsodium')) {
        dl('libsodium.' . PHP_SHLIB_SUFFIX);
}
$module = 'libsodium';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
$function = 'sodium_memzero';
$exit = 0;
if (extension_loaded($module)) {
        $str = $function($module);
} else {
        $str = "Module $module is not compiled into PHP";
        $exit = 255;
}
echo "$str\n";
exit($exit);
?>
