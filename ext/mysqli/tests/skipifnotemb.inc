<?php
  $driver = new mysqli_driver();
  if (!$driver->embedded) 
    die("skip test for with embedded server only");
?>
