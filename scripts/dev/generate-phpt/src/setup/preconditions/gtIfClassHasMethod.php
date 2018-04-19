<?php

/**
 * If use has requested a class check that method is specified
 *
 */
class gtIfClassHasMethod extends gtPreCondition {

  public function check( $clo) {
    return !$clo->hasOption('c') || $clo->hasOption('m');
  }

  public function getMessage() {
    return gtText::get('methodNotSpecified');
  }

}
?>
