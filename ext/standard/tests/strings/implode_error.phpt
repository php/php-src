--TEST--
--FILE--
<?php
/* only glue */
try {
    var_dump( implode("glue") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* NULL as pieces */
try {
    var_dump( implode("glue", NULL) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

/* integer as glue */
try {
    var_dump( implode(12, "pieces") );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
--EXPECTF--
implode(): If argument #1 ($separator) is of type string, argument #2 ($array) must be of type array, null given
implode(): Argument #2 ($array) must be of type array, null given
implode(): Argument #2 ($array) must be of type array, string given
