<?php
require __DIR__ . "/oom_rfc1867_skipif.inc";
if (!extension_loaded('mbstring')) {
    die("skip mbstring not available");
}
