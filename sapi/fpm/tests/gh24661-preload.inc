<?php
function preloaded_f() {
    try {
        return gettype(getmypid());
    } catch (Error $e) {
        return $e->getMessage();
    }
}
function preloaded_g() {
    echo var_export(true, true), "\n";
    echo gettype(getmypid()), "\n";
}
