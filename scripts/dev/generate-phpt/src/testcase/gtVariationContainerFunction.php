<?php

/**
 * Container for all possible variation test cases of functions
 */
class gtVariationContainerFunction extends gtVariationContainer {
  
  protected $function;
  protected $optionalSections;
  
  public function __construct($osl) {
    $this->optionalSections = $osl;
  }
  
  /**
   * Sets function being tested
   *
   * @param gtFunction $function
   */
  public function setFunction(gtFunction $function) {
    $this->function = $function;
  }
  
  
  /**
   * Constucts all possible variation testcases in array $this->variationTests
   *
   */
  public function constructAll() {
    
        
    $numberOfArguments = count($this->function->getMandatoryArgumentNames()) + count($this->function->getOptionalArgumentNames());
    for($i = 1; $i <= $numberOfArguments; $i++) {
      foreach ($this->dataTypes as $d) {
        $testCase = gtVariationTestCase::getInstance($this->optionalSections);
        $testCase->setUp($this->function, $i, $d);
        $testCase->constructTestCase();
        $this->variationTests[] = $testCase->toString();
      }
    }
  }  
}
?>