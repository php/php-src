<?php

/**
 * Basic test case for a PHP function
 *
 */
class gtBasicTestCaseFunction extends gtBasicTestCase {


  public function __construct($opt) {
    $this->optionalSections = $opt;
  }

  /**
   * Set the function name
   *
   * @param gtFunction $function
   */
  public function setFunction($function) {
    $this->subject = $function;
  }

  public function constructTestCase() {
    $this->constructCommonHeaders();
    
    $this->addBasicEcho();
    
    $this->constructSubjectCalls();
    
    $this->constructCommonClosing();
   
  }


  /**
   * Construct test case header
   *
   */
  public function testHeader() {
    //Opening section and start of test case array.
    $this->testCase[] = "--TEST--";
    $this->testCase[] = "Test function ".$this->subject->getName()."() by calling it with its expected arguments";
  }

  /**
   * Add the test section to call the function
   *
   */
  public function subjectCalls() {
    // Construct the argument list to pass to the function being tested
    $lists = $this->subject->getValidArgumentLists();

    foreach($lists as $list){

      $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
      $this->testCase[] = "var_dump(".$this->subject->getName()."( ".$list." ) );";
    }
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }

}
?>