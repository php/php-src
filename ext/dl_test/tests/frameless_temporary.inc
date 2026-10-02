<?php
var_dump(dl_test_frameless(7));
try {
    dl_test_frameless("nope");
    echo "no throw\n";
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
