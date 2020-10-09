<?php

spl_autoload_register(function($class) {
    var_dump($class);
    new Abc;
});
opcache_compile_file('preload_const_autoload_2.inc');
