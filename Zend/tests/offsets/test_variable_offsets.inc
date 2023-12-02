<?php

//$container var declaration in const generated file

// Read before write
try {
    echo "Read before write:\n";
    var_dump($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Write
try {
    echo "Write:\n";
    $container[$dimension] = 'v';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Read
try {
    echo "Read:\n";
    var_dump($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Read-Write
try {
    echo "Read-Write:\n";
    $container[$dimension] .= 'append';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Is
try {
    echo "isset():\n";
    var_dump(isset($container[$dimension]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "empty():\n";
    var_dump(empty($container[$dimension]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "Coalesce():\n";
    var_dump($container[$dimension] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "unset():\n";
    unset($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
