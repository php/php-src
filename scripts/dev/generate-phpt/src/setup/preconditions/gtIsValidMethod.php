<?php

/**
 * Check that teh method name is valid
 *
 */
class gtIsValidMethod extends gtPreCondition {

 public function check( $clo) {
    if($clo->hasOption('m') ) {
      $className = $clo->getOption('c');
      $class = new ReflectionClass($className);
      $methods = $class->getMethods();
      foreach($methods as $method) {
        if($clo->getOption('m') == $method->getName()) {
          return true;
        }
      }
      return false;
    }
    return true;
  }

  public function getMessage() {
    return gtText::get('unknownMethod');
  }
}
?>