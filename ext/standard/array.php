<?php

function array_find(array $array, callable $callback): mixed {
    foreach ($array as $key => $value) {
        if ($callback($value, $key)) {
            return $value;
        }
    }
}

function array_find_key(array $array, callable $callback): mixed {
    foreach ($array as $key => $value) {
        if ($callback($value, $key)) {
            return $key;
        }
    }
}

function array_any(array $array, callable $callback): bool {
    foreach ($array as $key => $value) {
        if ($callback($value, $key)) {
            return true;
        }
    }

    return false;
}

function array_all(array $array, callable $callback): bool {
    foreach ($array as $key => $value) {
        if (!$callback($value, $key)) {
            return false;
        }
    }

    return true;
}
