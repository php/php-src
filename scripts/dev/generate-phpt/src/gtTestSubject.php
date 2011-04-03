<?php
abstract class gtTestSubject {

  protected $optionalArgumentNames;
  protected $mandatoryArgumentNames;

  protected $extraArgumentList = '';
  protected $shortArgumentList = '';

  protected $allowedArgumentLists;

  protected $maximumArgumentList;
  
  protected $initialisationStatements;


  /** Return the list of all mandatory argument names
   *
   * @return array
   */
  public function getMandatoryArgumentNames() {
    return $this->mandatoryArgumentNames;
  }


  /**
   * Return the list of all optional argument names
   *
   * @return array
   */
  public function getOptionalArgumentNames() {
    return $this->optionalArgumentNames;
  }
  
  public function setArgumentLists() {
    $this->setValidArgumentLists();
    $this->setExtraArgumentList();
    $this->setShortArgumentList();
  }

  /**
   * Set the argument list to call the subject with. Adds one extra argument.
   *
   */
  public function setExtraArgumentList() {
    if(count ($this->mandatoryArgumentNames) > 0) {
      for( $i = 0; $i < count( $this->mandatoryArgumentNames ); $i++) {
        $this->extraArgumentList .= "\$".$this->mandatoryArgumentNames[$i].", ";
      }
    }
     
    if(count ($this->optionalArgumentNames) > 0) {
      for( $i = 0; $i < count( $this->optionalArgumentNames ); $i++) {
        $this->extraArgumentList .=  "\$".$this->optionalArgumentNames[$i].", ";
      }
    }

    $this->extraArgumentList= $this->extraArgumentList. "\$extra_arg";
  }
   

  /**
   * Return the list of arguments as it appears in the function call
   *
   * @return string - list of arguments
   */
  public function getExtraArgumentList() {
    return $this->extraArgumentList;
  }


  /**
   * Set the list of function arguments to be one less that the number of mandatory arguments
   *
   */
  public function setShortArgumentList() {

    if(count ($this->mandatoryArgumentNames) > 0) {
      for( $i = 0; $i < count( $this->mandatoryArgumentNames ) - 1; $i++) {
        $this->shortArgumentList .= "\$".$this->mandatoryArgumentNames[$i].", ";
      }
      $this->shortArgumentList = substr($this->shortArgumentList, 0, -2);
    }
  }


  /**
   * Return the short list of arguments
   *
   * @return string - list of arguments
   */
  public function getShortArgumentList() {
    return $this->shortArgumentList;
  }


  /**
   * Construct the list of all possible ways to call the subject (function or method)
   *
   */
  public function setValidArgumentLists() {
    $this->allowedArgumentLists[0] = '';
    if(count ($this->mandatoryArgumentNames) > 0) {
      for( $i = 0; $i < count( $this->mandatoryArgumentNames ); $i++) {
        $this->allowedArgumentLists[0] .= "\$".$this->mandatoryArgumentNames[$i].", ";
      }
    }
     
    if(count ($this->optionalArgumentNames) > 0) {
      for( $i = 0; $i < count( $this->optionalArgumentNames ); $i++) {
        $this->allowedArgumentLists[] = $this->allowedArgumentLists[$i]."\$".$this->optionalArgumentNames[$i].", ";
        $this->allowedArgumentLists[$i] = substr ($this->allowedArgumentLists[$i], 0, -2);
      }
    }

    $this->allowedArgumentLists[count($this->allowedArgumentLists) -1 ] = substr($this->allowedArgumentLists[count($this->allowedArgumentLists) -1 ], 0, -2);
  }


  /**
   * Return the array of all possible sets of method/function arguments
   *
   * @return unknown
   */
  public function getValidArgumentLists() {
    return $this->allowedArgumentLists;
  }


  /**
   * Returns the argument list with the greatest possible number of arguments.
   *
   * @return string
   */
  public function getMaximumArgumentList() {
    return end($this->allowedArgumentLists);
  }


  /**
   * Write initialisation statemenst for all the variables that might be used
   *
   */
  public function setInitialisationStatements() {
    if(count ($this->mandatoryArgumentNames) > 0) {
      foreach( $this->mandatoryArgumentNames as $name) {
        $this->initialisationStatements[] = "\$".$name." = ";
      }
    }
    if(count ($this->optionalArgumentNames) > 0) {
      foreach( $this->optionalArgumentNames as $name) {
        $this->initialisationStatements[] = "\$".$name." = ";
      }
    }
  }
  
  /**
   * Return the initialisation statements
   *
   * @return unknown
   */
  public function getInitialisationStatements() {
    return $this->initialisationStatements;
  }
}
?>
