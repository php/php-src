--TEST--
PFA RFC examples: "Incompatible functions" section
--FILE--
<?php

try {
    (function ($f) { $f(?); })("func_get_arg");
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot call func_get_arg() dynamically
