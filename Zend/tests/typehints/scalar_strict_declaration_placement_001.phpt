--TEST--
Test strict declaration being first operation only
--FILE--
<?php

function takes_int(int $x) {
    global $errored;
    if ($errored) {
        echo "Failure!", PHP_EOL;
        $errored = FALSE;
    } else {
        echo "Success!", PHP_EOL;
    }
}

?>
<?php

declare(strict_types=1);
var_dump(takes_int(32));

?>
--EXPECTF--
Fatal error: strict_types declaration must be the very first statement in the script in %s on line %d
