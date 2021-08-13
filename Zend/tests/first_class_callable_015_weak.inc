<?php

function do_weak_call(Closure $fn) {
    $fn("42");
}
