<?php

package "pkg";

try {
    var_dump(strlen(2));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
