<?php

/**
 * Check that either a method or a function is specified
 *
 */
class gtIsSpecifiedFunctionOrMethod extends gtPreCondition {
  
  public function check( $clo) {
    if($clo->hasOption('f') || $clo->hasOption('m')) {
    
        return true;
      }
    return false;
  }
  
  public function getMessage() {
    return gtText::get('functionOrMethodNotSpecified');
  }
}
?>