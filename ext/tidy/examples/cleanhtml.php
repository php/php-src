<?php

    /*
     * cleanhtml.php
     *
     * A simple script to clean and repair HTML,XHTML,PHP,ASP,etc. documents
     * if no file is provided, it reads from standard input.
     *
     * By: John Coggeshall <john@php.net>
     *
     * Usage: php cleanhtml.php [filename]
     *
     */
    
    $tidy = tidy_create();
    
    if(!isset($_SERVER['argv'][1])) {
      $data = file_get_contents("php://stdin");
      tidy_parse_string($tidy, $data);
    } else {
       tidy_parse_file($tidy, $_SERVER['argv'][1]);
    }
    
    tidy_clean_repair($tidy);
    
    if(tidy_warning_count($tidy) ||
       tidy_error_count($tidy)) {
        
        echo "\n\nThe following errors or warnings occured:\n";
        echo tidy_get_error_buffer($tidy);
        echo "\n";
    }
    
    echo tidy_get_output($tidy);
    
?>
    
        
     
     