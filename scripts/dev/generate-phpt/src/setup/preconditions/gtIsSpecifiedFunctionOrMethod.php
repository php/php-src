<?php

/**
 * Check that either a method or a function is specified
 *
 */
class gtIsSpecifiedFunctionOrMethod extends gtPreCondition {

  public function check( $clo) {
    return $clo->hasOption('f') || $clo->hasOption('m');
  }

  public function getMessage() {
    return gtText::get('functionOrMethodNotSpecified');
  }
}
?>
