<?php

/**
 * Class for simple errors - one too many args and one too few
 */

abstract class gtErrorTestCase extends gtTestCase {

  protected $shortArgumentList = '';
  protected $longArgumentList = '';


  /**
   * Return instance of either method or function error test case
   *
   * @param string $type
   * @return test case object
   */
  public static function getInstance($optionalSections, $type = 'function') {
     
    if($type == 'function') {
      return new gtErrorTestCaseFunction($optionalSections);
    }
    if($type =='method') {
      return new gtErrorTestCaseMethod($optionalSections);
    }

  }

  public function getShortArgumentList() {
    return $this->shortArgumentList;
  }

  public function getLongArgumentList() {
    return $this->longArgumentList;
  }
  
  public function constructSubjectCalls() {
    $this->argInit();
    
    //Initialise the additional argument
    $this->testCase[] = "\$extra_arg = ";
    
    $this->subjectCalls();
  }
  
 public function addErrorEcho() {
    $this->testCase[] = "echo \"*** Test by calling method or function with incorrect numbers of arguments ***\\n\";";
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
  }
}

?>