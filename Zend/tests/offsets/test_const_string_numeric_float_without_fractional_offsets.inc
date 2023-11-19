<?php

// Read before write
try {
    echo "Read before write:\n";
    var_dump($container["6.0"]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Write
try {
    echo "Write:\n";
    $container["6.0"] = 'v';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Read
try {
    echo "Read:\n";
    var_dump($container["6.0"]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Read-Write
try {
    echo "Read-Write:\n";
    $container["6.0"] .= 'append';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Is
try {
    echo "isset():\n";
    var_dump(isset($container["6.0"]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "empty():\n";
    var_dump(empty($container["6.0"]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "Coalesce():\n";
    var_dump($container["6.0"] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
