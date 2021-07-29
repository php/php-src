<?php
spl_autoload_register(function($class) {
    // Autoloader should not get called.
    echo "Trying to autoload $class\n";
});
opcache_compile_file(__DIR__ . '/preload_variance.inc');
