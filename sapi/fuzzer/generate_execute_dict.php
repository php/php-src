<?php

/* Internal functions and classes are looked up through hash tables, so the fuzzer cannot
 * learn their names from comparisons. Combine them with the keywords of the parser dictionary. */
$dict = file_get_contents(__DIR__ . "/dict/parser");
foreach (get_defined_functions()['internal'] as $function) {
    $dict .= "\"" . addcslashes($function, "\\\"") . "\"\n";
}
foreach (array_merge(get_declared_classes(), get_declared_interfaces()) as $class) {
    $dict .= "\"" . addcslashes($class, "\\\"") . "\"\n";
}
file_put_contents(__DIR__ . "/dict/execute", $dict);
