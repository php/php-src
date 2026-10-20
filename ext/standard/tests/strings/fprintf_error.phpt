--TEST--
Test fprintf() function (errors)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* zero argument */
try {
    var_dump( fprintf() );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* scalar argument */
try {
    var_dump( fprintf(3) );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* NULL argument */
try {
    var_dump( fprintf(NULL) );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing Error Conditions ***
ArgumentCountError: fprintf() expects at least 2 arguments, 0 given
ArgumentCountError: fprintf() expects at least 2 arguments, 1 given
ArgumentCountError: fprintf() expects at least 2 arguments, 1 given
Done
