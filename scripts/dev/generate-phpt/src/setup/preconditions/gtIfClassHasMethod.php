<?php

/**
 * If use has requested a class check that method is specified
 *
 */
class gtIfClassHasMethod extends gtPreCondition {
  
  public function check( $clo) {
    if($clo->hasOption('c')) {
      if(!$clo->hasOption('m')) {
        return false;
      }
      return  true;
    }
    return true;
  }
  
  public function getMessage() {
    return gtText::get('methodNotSpecified');
  }

}
?>