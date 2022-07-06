<?php
if (!extension_loaded('ftp')) die('skip ftp extension not loaded');
if (!function_exists('pcntl_fork')) die('skip fork not available');
if (!empty($ssl) && !extension_loaded('openssl')) die('skip openssl extension not loaded');
?>
