<?php

/**
 * Error test case for a PHP method
 *
 */
class gtErrorTestCaseMethod extends gtErrorTestCase {

  public function __construct($opt) {
    $this->optionalSections = $opt;
  }
  private $method;

  /**
   * Set the method name
   *
   * @param string $method
   */
  public function setMethod($method) {
    $this->subject = $method;
  }


  /**
   * Construct the test case as an array of strings
   *
   */
  public function constructTestCase() {
    $this->constructCommonHeaders();
    
    $this->addErrorEcho();
    
    $this->constructorArgInit();
    $this->constructorCreateInstance();
    
    $this->constructSubjectCalls();
    
    $this->constructCommonClosing();
  }
 
  public function testHeader() {
    $this->testCase[] = "--TEST--";
    $this->testCase[] = "Test class ".$this->subject->getClassName()." method ".$this->subject->getName()."() by calling it more than or less than its expected arguments";
  }
 
  public function subjectCalls() {

    // Construct the argument list to pass to the method being tested
    $list = $this->subject->getExtraArgumentList();
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
    $this->testCase[] = "var_dump(".$this->subject->getName()."( ".$list." ) );";
    
    $list = $this->subject->getShortArgumentList();
    $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
    $this->testCase[] = "var_dump(".$this->subject->getName()."( ".$list." ) );";
  
  }
}
?>