<?php

function trycatch_dump(...$tests) {
    foreach ($tests as $test) {
        try {
            var_dump($test());
        }
        catch (\Error $e) {
            echo '!! [' . get_class($e) . '] ' . $e->getMessage() . "\n";
        }
    }
}
