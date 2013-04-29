<?php

    /*
     * cleanhtml.php
     *
     * A simple script to clean and repair HTML,XHTML,PHP,ASP,etc. documents
     * if no file is provided, it reads from standard input.
     *
     * NOTE: Works only with tidy for PHP 4.3.x, for tidy in PHP 5 see cleanhtml5.php
     *
     * By: John Coggeshall <john@php.net>
     *
     * Usage: php cleanhtml.php [filename]
     *
     */
    
    if(!isset($_SERVER['argv'][1])) {
      $data = file_get_contents("php://stdin");
      tidy_parse_string($data);
    } else {
       tidy_parse_file($_SERVER['argv'][1]);
    }
    
    tidy_clean_repair();
    
    if(tidy_warning_count() ||
       tidy_error_count()) {
        
        echo "\n\nThe following errors or warnings occurred:\n";
        echo tidy_get_error_buffer();
        echo "\n";
    }
    
    echo tidy_get_output();
    
?>
    
        
     
     