--TEST--
Test fprintf() function (errors)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* zero argument */
try {
    var_dump( fprintf() );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* scalar argument */
try {
    var_dump( fprintf(3) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* NULL argument */
try {
    var_dump( fprintf(NULL) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
*** Testing Error Conditions ***
fprintf() expects at least 2 arguments, 0 given
fprintf() expects at least 2 arguments, 1 given
fprintf() expects at least 2 arguments, 1 given
Done
