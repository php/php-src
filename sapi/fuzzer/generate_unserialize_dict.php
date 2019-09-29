<?php

$dict = "";
foreach (get_declared_classes() as $class) {
    $len = strlen($class);
    $dict .= "\"$len:\\\"$class\\\"\"\n";
}

file_put_contents(__DIR__ . "/dict/unserialize", $dict);
