<?php

$envUser = getenv("USER") ?: getenv("USERNAME");

if ($envUser === false) {
    die("skip This test is not executed without environment variables USER/USERNAME");
}

if (function_exists("posix_geteuid")) {
    if (posix_geteuid() === 0) {
        die("skip This test is not executed with root user");
    }
}

?>
