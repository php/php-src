<?php

/**
 * Error test case for a PHP function
 *
 */
class gtErrorTestCaseFunction extends gtErrorTestCase {

  public function __construct($opt) {
    $this->optionalSections = $opt;
  }

  /**
   * Set the function name
   *
   * @param string $function
   */
  public function setFunction($function) {
    $this->subject = $function;
  }


  /**
   * Construct the test case as an array of strings
   *
   */
  public function constructTestCase() {
    $this->constructCommonHeaders();
    
    $this->addErrorEcho();
      
    $this->constructSubjectCalls();
    
    $this->constructCommonClosing();
    
  }


  public function testHeader() {
    $this->testCase[] = "--TEST--";
    $this->testCase[] = "Test function ".$this->subject->getName()."() by calling it more than or less than its expected arguments";
  }

  public function subjectCalls() {
    // Construct the argument lists to pass to the function being tested
    $list = $this->subject->getExtraArgumentList();
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
    $this->testCase[] = "var_dump(".$this->subject->getName()."( ".$list." ) );";

    $list = $this->subject->getShortArgumentList();
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
    $this->testCase[] = "var_dump(".$this->subject->getName()."( ".$list." ) );";
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }

}
?>