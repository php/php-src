<?php

function test_oscode($gzhex) {
    $oscode = detect_oscode();

    $expect = $oscode;
    $actual = substr($gzhex, 18, 2);
    if ($expect == $actual) {
        return 'ok';
    } else {
        return [
            'expect' => $expect,
            'actual' => $actual,
        ];
    }
}

function detect_oscode() {
    if (substr(PHP_OS, 0, 3) == "WIN") {
        return "0a";
    } else if (PHP_OS == "Darwin") {
        return "13";
    } else {
        return "03";
    }
}

function filter_oscode($gzhex, $default) {
    return substr_replace($gzhex, $default, 18, 2);
}
