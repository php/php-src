<?php

/**
 * Retrieves code snippets for adding to test cases
 * 
 */
class gtCodeSnippet
{

  /**
   * get the code snippet and initialise an array with it
   *
   * @param string $name
   * @return array
   */
  public static function get($name) {
    
    $filename = dirname(__FILE__) . '/codeSnippets/' . $name . '.txt';

    if (!file_exists($filename)) {
      throw new LogicException('The code snippet ' . $name . ' does not exist');
    }
    
    $lines = file($filename);
    foreach($lines as $l) {
      $array[] = rtrim($l);
    }
    return $array;
  }
  
  
  /**
   * Append the code snippet on to an existing array
   *
   * @param string $name
   * @param array $array
   * @return array
   */
  public static function append($name, $array) {
    $filename = dirname(__FILE__) . '/codeSnippets/' . $name . '.txt';

    if (!file_exists($filename)) {
      throw new LogicException('The code snippet ' . $name . ' does not exist');
    }

    $text =  file($filename);
    foreach ($text as $t) {
      $array[] = rtrim($t);
    }
    
    return $array;
  }
  
  
  /**
   * Appends blank entries on to an array
   *
   * @param int $numberOfLines
   * @param array $array
   * @return array
   */
  public static function appendBlankLines($numberOfLines, $array) {

    for ($i=0; $i< $numberOfLines; $i++) {
      $array[] = "";
    }
    
    return $array;
  }
  
}
?>