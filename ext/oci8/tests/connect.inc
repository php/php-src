<?php

include "details.inc";

/* 
 * You should have privileges to create tables in this schema 
 * 
 * */
/*
$schema     = "system"; 
*/

$table_name = "tb".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5);
$type_name = strtoupper("tp".substr(str_replace(Array(".", "-"), "_", php_uname("n")), 0, 5));
 
    if (!empty($dbase)) {
        $c = ocilogon($user,$password,$dbase);
    }
    else {
        $c = ocilogon($user,$password);
    }

    if (!empty($schema)) {
        $schema = $schema.".";
    }
    else {
        $schema = '';
    }

?>
