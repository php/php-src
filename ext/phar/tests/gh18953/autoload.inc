<?php

spl_autoload_register(function ($class) {
    $base_dir = __DIR__ . '/src/';

    $file = $base_dir . str_replace('\\', '/', $class) . '.inc';
    if (file_exists($file)) {
        require $file;
    }
});
