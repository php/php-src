<?php

/* 
 * Please, change user, password and dbase to match your configuration.
 * 
 * */

$user       = "user";
$password   = "pass";
$dbase      = "base"; 

/* 
 * You should have privileges to create tables in this schema 
 * 
 * */

$schema     = "system"; 

/*
 * Remove the last line in skipif.inc to run tests
 *
 * */


    if (!empty($dbase)) {
        $connection = ocilogon($user,$password,$dbase);
    }
    else {
        $connection = ocilogon($user,$password);
    }

    if (!empty($schema)) {
        $schema = $schema.".";
    }
    else {
        $schema = '';
    }

?>
