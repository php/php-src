<?php

/**
 * Container for all possible variation test cases
 */
abstract class gtVariationContainer {
  
  protected $variationTests;
  
  protected $dataTypes = array (
                         'array',
                         'boolean',
                         'emptyUnsetUndefNull',
                         'float',
                         'int',
                         'object',
                         'string',
                         );
  
  
                     
  /**
   * Return an instance of a containers for either function or method tests
   *
   * @param string $type
   * @return variation test container
   */
   public static function getInstance ($optionalSections, $type = 'function') {
    
    if($type == 'function') {
      return new gtVariationContainerFunction($optionalSections);
    }
    if($type =='method') {
      return new gtVariationContainerMethod($optionalSections);
    }
    
  }
  
  
  public function constructAll() {
  }
  
  
  /**
   * Returns all varaition tests as an array of arrays
   *
   * @return string
   */
  public function getVariationTests() {
    return $this->variationTests;
  }
  
}
?>