<?php

// implicitly weak code

function takes_int(int $x) {
    echo "Success!";
}

takes_int(1.0); // succeeds in weak mode
