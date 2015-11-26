<?php

/**
 * List of preconditions.
 *
 */
class gtPreConditionList {
  
  private $preConditions = array(
      'gtIsSpecifiedTestType',
      'gtIsSpecifiedFunctionOrMethod',  
      'gtIfClassHasMethod',
      'gtIsValidClass',
      'gtIsValidFunction',
      'gtIsValidMethod',
  );


  /**
   * Create an instance of each pre-condition and run their check methods
   *
   */
  public function check($clo) {
    foreach ($this->preConditions as $preCon) {
      $checkThis = new $preCon;
      if(!$checkThis->check($clo)) {
        echo $checkThis->getMessage();
        die(gtText::get('help'));
      }
    }
  }
}
?>