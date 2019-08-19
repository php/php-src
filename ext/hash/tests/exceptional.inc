<?php

    function trycatch_dump($x) {
        try {
            $result = $x();
            var_dump($result);

        }
        catch (\Throwable $ex) {
            echo '[' . get_class($ex) . '] ' . $ex->getMessage() . "\n";
        }
    }
