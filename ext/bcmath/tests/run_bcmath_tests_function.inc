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
                echo $firstTerm,
                    " $symbol ",
                    str_pad($secondTerm, STRING_PADDING),
                    " = ",
                    $bcmath_function($firstTerm, $secondTerm, $scale),
                    "\n";
            }
            echo "\n";
        }
    }
}
