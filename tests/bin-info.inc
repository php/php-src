<?php
// Used to print php binary info used to run
// test scripts.
// Called from run-tests.php

function dowrite($str)
{
    global $term_bold, $term_norm;
    $str = str_replace("%b", $term_bold, $str);
    $str = str_replace("%B", $term_norm, $str);
    print $str;
}

function dowriteln($str)
{
    dowrite("$str\n");
}

dowriteln("PHP SAPI: ".PHP_SAPI);
dowriteln("PHP Version: ".PHP_VERSION);
?>