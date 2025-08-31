<?php

set_error_handler(function($_, $m) {
    static $x;
    var_dump($m);
});

opcache_compile_file(__DIR__ . '/preload_error_handler_ind.inc');
