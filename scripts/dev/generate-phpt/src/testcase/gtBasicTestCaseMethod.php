<?php

/**
 * Class for basic test case construction for class methods
 */
class gtBasicTestCaseMethod extends gtBasicTestCase {

  public function __construct($opt) {
    $this->optionalSections = $opt;
  }

  /**
   * Set the method 
   *
   * @param gtMethod $method
   */
  public function setMethod($method) {
    $this->subject = $method;
  }
  
public function constructTestCase() {
    $this->constructCommonHeaders();

    $this->addBasicEcho();
    
    $this->constructorArgInit();
    $this->constructorCreateInstance();
    
    $this->constructSubjectCalls();
    
    $this->constructCommonClosing();
    
  }

  public function testHeader() {
    $this->testCase[] = "--TEST--";
    $this->testCase[] = "Test class ".$this->subject->getClassName()." method  ".$this->subject->getName()."() by calling it with its expected arguments";
    
  }
  
  public function subjectCalls() {
    $lists = $this->subject->getValidArgumentLists();

    foreach($lists as $list){
      $this->testCase[] = "var_dump( \$class->".$this->subject->getName()."( ".$list." ) );";
      $this->testCase = gtCodeSnippet::appendBlankLines(1, $this->testCase );
    }
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );    
  }

}
?>