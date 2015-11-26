<?php
/**
 * Container for all possible variation test cases for a method
 */
class gtVariationContainerMethod extends gtVariationContainer {
  
  protected $method;
  protected $optionalSections;
  
  public function __construct($osl) {
    $this->optionalSections = $osl;
  }
  
  
  /**
   * Sets the method to be tested
   *
   * @param gtMethod $method
   */
  public function setMethod(gtMethod $method) {
    $this->method = $method;
  }
  
  
  /**
   * Constructs all variation tests in $this_variationTests
   *
   */
  public function constructAll() {
    
    $numberOfArguments = count($this->method->getMandatoryArgumentNames()) + count($this->method->getOptionalArgumentNames());
    
    for($i = 1; $i <= $numberOfArguments; $i++) {
      
      foreach ($this->dataTypes as $d) {
        
        $testCase = gtVariationTestCase::getInstance($this->optionalSections, 'method');
        $testCase->setUp($this->method, $i, $d);
        $testCase->constructTestCase();
        $this->variationTests[] = $testCase->toString();
        
      }
    }
  }  
}
?>