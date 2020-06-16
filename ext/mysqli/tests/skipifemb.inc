<?php
    $driver = new mysqli_driver();
    if ($driver->embedded)
        die("skip test doesn't run with embedded server");
?>
