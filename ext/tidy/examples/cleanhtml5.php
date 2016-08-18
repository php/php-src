<?php

    /*
     * cleanhtml5.php
     *
     * A simple script to clean and repair HTML,XHTML,PHP,ASP,etc. documents
     * if no file is provided, it reads from standard input.
     *
     * NOTE: Works only with tidy for PHP 5, for tidy in PHP 4.3.x see cleanhtml.php
     *
     * By: John Coggeshall <john@php.net>
     *
     * Usage: php cleanhtml5.php [filename]
     *
     */
    
    if(!isset($_SERVER['argv'][1])) {
      $data = file_get_contents("php://stdin");
      $tidy = tidy_parse_string($data);
    } else {
      $tidy = tidy_parse_file($_SERVER['argv'][1]);
    }
    
    $tidy->cleanRepair();
    
    if(!empty($tidy->errorBuffer)) {
        
        echo "\n\nThe following errors or warnings occurred:\n";
        echo "{$tidy->errorBuffer}\n";
        
    }
    
    echo $tidy;
    
?>
