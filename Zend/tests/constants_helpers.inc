<?php

    # provides functions for debugging constants and define
    # statements

    function tchelper_define($key, $value) {
        printf(">> define(%s, %s);\n", is_string($key) ? sprintf('"%s"', $key) : gettype($key), gettype($value));
        try {
            echo (define($key, $value) ? 'true' : 'false') . PHP_EOL;
        }
        catch (\Throwable $e) {
            echo get_class($e) . ' :: ' . $e->getMessage() .  PHP_EOL;
        }

        echo PHP_EOL;
    }