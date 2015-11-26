<?php

/**
 * Writes a single test case to a file
 *
 */
class gtTestCaseWriter {
  
  public static function write($name, $string, $type, $count = 0) {
    if ($type  == 'b') {
    $fileName = $name."_basic.phpt";
    }
    
    if ($type  == 'e') {
     $fileName = $name."_error.phpt";
    }
    
    if ($type  == 'v') {
      $fileName = $name."_variation".$count.".phpt";
    }
    
    $fh = fopen($fileName, 'w');
    fwrite ($fh, $string);
    fclose($fh);
  }
}
?>