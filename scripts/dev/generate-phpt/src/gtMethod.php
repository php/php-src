<?php

/**
 * Class for method under test (see gtFunction for non-OO tests)
 */
class gtMethod  extends gtTestSubject {

  private $className;
  private $methodName;
  private $constructorArgumentNames;
  private $constructorArgumentList = '';
  private $constructorInitialisationStatements;



  /**
   * Construct gtMethod object from the class and method names
   *
   * @param string $className
   * @param string $methodName
   */
  public function __construct($className, $methodName) {
    $this->className = $className;
    $this->methodName = $methodName;
  }


  /**
   * Set the names of the class constructor arguments. Take only mandatory argument names.
   *
   */
  public function setConstructorArgumentNames() {
    $reflectionClass = new ReflectionClass($this->className);
    $constructor = $reflectionClass->getConstructor();
    foreach($constructor->getParameters() as $i => $param) {
      //if(!$param->isOptional()) {
        $this->constructorArgumentNames[] = $param->getName();
      //}
    }
  }

  
  /**
   * Set the names of the mandatory and optional arguments to the method
   *
   */
  public function setArgumentNames() {

    $methodClass  = new reflectionMethod($this->className, $this->methodName);
    $parameters = $methodClass->getParameters();

    foreach ($methodClass->getParameters() as $i => $param) {
      if($param->isOptional()) {
        $this->optionalArgumentNames[] = $param->getName();
      } else {
        $this->mandatoryArgumentNames[] = $param->getName();
      }

    }
  }


  /**
   * Return the list of constructor argument names
   *
   * @return array
   */
  public function getConstructorArgumentNames() {
    return $this->constructorArgumentNames;
  }

  /**
   * Return the name of the method
   *
   * @return string
   */
  public function getName() {
    return $this->methodName;
  }


  /**
   * Return the name of the class
   *
   * @return string
   */
  public function getClassName() {
    return $this->className;
  }
  
  /**
   * Set the list of arguments to be passed to the constructor
   *
   */
  public function setConstructorArgumentList() {
    if(count ($this->constructorArgumentNames) > 0) {
      
      for( $i = 0; $i < count( $this->constructorArgumentNames ); $i++) {
        $this->constructorArgumentList .= "\$".$this->constructorArgumentNames[$i].", ";
      }
      $this->constructorArgumentList = substr($this->constructorArgumentList, 0, -2);
    }
  }


  /**
   * Return the list of the arguments to be passed to the constructor
   *
   * @return string
   */
  public function getConstructorArgumentList() {
    return $this->constructorArgumentList;
  }

  
  /**
   * Set up the source statements that initialise constructor arguments;
   *
   */
  public function setConstructorInitStatements() {
    if(count ($this->constructorArgumentNames) > 0) {
      foreach( $this->constructorArgumentNames as $name) {
        $this->constructorInitialisationStatements[] = "\$".$name." = ";
      }
    }

  }

  
  /**
   * Return the constructor initialisation statements
   *
   * @return array
   */
  public function getConstructorInitStatements() {
    return $this->constructorInitialisationStatements;
  }
}
?>