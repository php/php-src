<?php

ob_start();
var_dump(ftell(STDIN));
var_dump(ftell(STDERR));
var_dump(ftell(STDOUT));
var_dump(fread(STDIN, 1));
var_dump(fwrite(STDERR, "a"));
var_dump(fwrite(STDOUT, "a"));
file_put_contents(__DIR__ . '/std_streams_closed.txt', ob_get_clean());
