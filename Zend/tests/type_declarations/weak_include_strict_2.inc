<?php

declare(strict_types=1);

function takes_int(int $x) {
    echo "Success!";
}

takes_int(1.0); // fails in strict mode
