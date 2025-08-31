<?php

if (getenv('RUNTIME')) {
    function g(): int {
        return 2;
    }
}
