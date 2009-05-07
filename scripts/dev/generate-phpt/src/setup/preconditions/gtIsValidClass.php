<?php

/**
 * Check that the class name is valid
 *
 */
class gtIsValidClass extends gtPreCondition {

  public function check( $clo) {
    if($clo->hasOption('c') ) {
      $className = $clo->getOption('c');
      if( in_array( $className, get_declared_classes() ) ) {
        return true;
      }
      return false;
    }
    return true;
  }

  public function getMessage() {
    return gtText::get('unknownClass');
  }
}
?>