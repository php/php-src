<?php

/**
 * parent class for preconditions
 *
 */
abstract class gtPreCondition {
  
  abstract public function check($clo); 
  
  abstract public function getMessage(); 
  
}
?>