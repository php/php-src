<?php
    if (!extension_loaded("dbx")) die("skip"); 
    include_once("dbx_test.p");
    if ($connection===NULL) die("skip connection not specified");
    die("info using $module_name");
?>