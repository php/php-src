<?php

$mb_used = (int) ceil(memory_get_usage() / (1024 ** 2));
ini_set('memory_limit', ($mb_used + 1) . 'M');

$class = $argv[1];
$objects = [];

try {
    while (true) {
        $rc = new ReflectionClass($class);
        $objects[] = $rc->newInstanceWithoutConstructor();
    }
} catch (Throwable) {
}
