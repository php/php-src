<?php

/**
 * Class for variation tests for a PHP method
 */
class gtVariationTestCaseMethod extends gtVariationTestCase {

  protected $subject;
  protected $argumentNumber;
  protected $variationData;
  protected $testCase;

  public function __construct($opt) {
    $this->optionalSections = $opt;
  }

  /**
   * Set data neede to construct variation tests
   *
   * @param gtMethod $method
   * @param string $argumentNumber
   * @param string $variationData
   */
  public function setUp(gtMethod $method, $argumentNumber, $variationData) {
    $this->subject = $method;
    $this->argumentNumber = $argumentNumber;
    $this->variationData = $variationData;
  }


  /**
   * Constructs the test case as a array of strings
   *
   */
  public function constructTestCase() {
    $this->constructCommonHeaders();
    
    $this->addVariationEcho();
    
    $this->constructorArgInit();
    $this->constructorCreateInstance();
    
    $this->constructSubjectcalls();
    $this->constructCommonClosing();
    
  }

  public function testHeader() {
    $this->testCase[] = "--TEST--";
    $this->testCase[] = "Test class ".$this->subject->getClassName()." method ".$this->subject->getName()."() by substituting argument ".$this->argumentNumber." with ".$this->variationData." values.";
  }

  public function subjectCalls() {
    $this->testCase = gtCodeSnippet::append('loopStart', $this->testCase);
    // Construct the argument list to pass to the method being tested
    $argumentList = explode(",", $this->subject->getMaximumArgumentList());
    $argumentList[$this->argumentNumber -1 ] = "\$var ";
    $list = implode(", ", $argumentList);


    $this->testCase[] = "  var_dump(\$class->".$this->subject->getName()."( ".$list." ) );";
    $this->testCase = gtCodeSnippet::append('loopClose', $this->testCase);

  }

}

?>