<?php

/**
 * Class for basic test case construction
 */

abstract class gtBasicTestCase extends gtTestCase {

  protected $subject;


  /**
   * Returns an instance of a test case for a method or a function
   *
   * @param string $type
   * @return test case object
   */
  public static function getInstance($optionalSections, $type = 'function') {
    if($type == 'function') {
      return new gtBasicTestCaseFunction($optionalSections);
    }
    if($type =='method') {
      return new gtBasicTestCaseMethod($optionalSections);
    }
  }
  
  public function constructSubjectCalls() {
        $this->argInit();
        $this->subjectCalls();
  }
  
  public function addBasicEcho() {
    $this->testCase[] = "echo \"*** Test by calling method or function with its expected arguments ***\\n\";";
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
  }
}
?>