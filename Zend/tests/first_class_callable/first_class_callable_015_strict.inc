<?php declare(strict_types=1);

function do_strict_call(Closure $fn) {
    $fn("42");
}
