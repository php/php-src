<?php

/**
 * Check that b|c|v is specified
 *
 */
class gtIsSpecifiedTestType extends gtPreCondition {
  
  public function check( $clo) {
    if($clo->hasOption('b') || $clo->hasOption('e') || $clo->hasOption('v') ) {
    
        return true;
      }
    return false;
  }
  
  public function getMessage() {
    return gtText::get('testTypeNotSpecified');
  }
}
?>