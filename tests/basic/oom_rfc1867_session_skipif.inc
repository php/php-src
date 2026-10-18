<?php
require __DIR__ . "/oom_rfc1867_skipif.inc";
if (!extension_loaded('session')) {
    die("skip session not available");
}
