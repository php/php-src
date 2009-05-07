<?php

/**
 * Class reperesents a single PHP function.
 *
 */
class gtFunction extends gtTestSubject {

  private $functionName;
  
  /**
   * Set the name of the name of the function
   *
   * @param string $functionName
   */
  public function __construct($functionName)  {
    $this->functionName = $functionName;
  }


  /**
   * Get the names of function argments and initialise mandatory and optional argument arrays
   *
   */
  public function setArgumentNames() {
    $function= new ReflectionFunction($this->functionName);

    foreach ($function->getParameters() as $i => $param) {
      if($param->isOptional()) {
        $this->optionalArgumentNames[] = $param->getName();
      } else {
        $this->mandatoryArgumentNames[] = $param->getName();
      }
    }
  }


  /**
   * Return the name of the function
   *
   * @return string
   */
  public function getName() {
    return $this->functionName;
  }

}
?>