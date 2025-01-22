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
    $container[$dimension] = 5;
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
    $container[$dimension] += 20;
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
    echo "null coalesce:\n";
    var_dump($container[$dimension] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Fetch reference
try {
    echo "Reference to dimension:\n";
    $r = &$container[$dimension];
    echo "Value of reference:\n";
    var_dump($r);
    $r = 100;
    echo "Value of container dimension after write to reference (should be int(100) if successful):\n";
    var_dump($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Unset
try {
    echo "unset():\n";
    unset($container[$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Nested read
try {
    echo "Nested read:\n";
    var_dump($container[$dimension][$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Nested write
try {
    echo "Nested write:\n";
    $container[$dimension][$dimension] = 5;
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Nested Read-Write
try {
    echo "Nested Read-Write:\n";
    $container[$dimension][$dimension] += 25;
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Nested Is
try {
    echo "Nested isset():\n";
    var_dump(isset($container[$dimension][$dimension]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "Nested empty():\n";
    var_dump(empty($container[$dimension][$dimension]));
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    echo "Nested null coalesce:\n";
    var_dump($container[$dimension][$dimension] ?? 'default');
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
// Nested unset
try {
    echo "Nested unset():\n";
    unset($container[$dimension][$dimension]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
