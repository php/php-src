<?php

const STRING_PADDING = 30;

function run_bcmath_tests(
    $firstTerms,
    $secondTerms,
    $symbol,
    $bcmath_function
) {
    $scales = [0, 10];
    foreach ($scales as $scale) {
        foreach ($firstTerms as $firstTerm) {
            echo "Number \"$firstTerm\" (scale $scale)\n";
            foreach ($secondTerms as $secondTerm) {
                try {
                    $ret = $bcmath_function($firstTerm, $secondTerm, $scale);
                } catch (Throwable $e) {
                    $ret = $e->getMessage();
                }
                echo $firstTerm,
                    " $symbol ",
                    str_pad($secondTerm, STRING_PADDING),
                    " = ",
                    $ret,
                    "\n";
            }
            echo "\n";
        }
    }
}
