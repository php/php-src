<?php

/**
 * Check that b|c|v is specified
 *
 */
class gtIsSpecifiedTestType extends gtPreCondition {

  public function check( $clo) {
    return $clo->hasOption('b') || $clo->hasOption('e') || $clo->hasOption('v');
  }
  
  public function getMessage() {
    return gtText::get('testTypeNotSpecified');
  }
}
?>
