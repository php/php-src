<?php

/**
 * Check that the function name is valid
 *
 */
class gtIsValidFunction extends gtPreCondition {

  public function check( $clo) {
    if($clo->hasOption('f') ) {
      $function = $clo->getOption('f');
      $functions = get_defined_functions();
      if( in_array( $function, $functions['internal'] ) ) {
        return true;
      }
      return false;
    }
    return true;
  }

  public function getMessage() {
    return gtText::get('unknownFunction');
  }
}
?>