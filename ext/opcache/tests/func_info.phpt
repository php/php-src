--TEST--
Test that return types in zend_func_info.c match return types in stubs
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

$contents = "<?php\n";

$contents .= "function test() {\n";
foreach (get_defined_functions()["internal"] as $function) {
    if (in_array($function, ["extract", "compact", "get_defined_vars"])) {
        continue;
    }
    $contents .= "    \$result = {$function}();\n";
}
$contents .= "}\n";

file_put_contents("func_info_generated.php", $contents);

require_once("func_info_generated.php");

?>
--CLEAN--
<?php

unlink("func_info_generated.php");

?>
--EXPECT--
