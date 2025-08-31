--TEST--
Bug #28817 (properties in extended class)
--EXTENSIONS--
dom
--FILE--
<?php

class z extends domDocument{
    /** variable can have name */
    public $p_array;
    public $p_variable;

    function __construct(){
        $this->p_array[] = 'bonus';
        $this->p_array[] = 'vir';
        $this->p_array[] = 'semper';
        $this->p_array[] = 'tiro';

        $this->p_variable = 'Cessante causa cessat effectus';
    }
}

$z=new z();
var_dump($z->p_array);
var_dump($z->p_variable);
?>
--EXPECT--
array(4) {
  [0]=>
  string(5) "bonus"
  [1]=>
  string(3) "vir"
  [2]=>
  string(6) "semper"
  [3]=>
  string(4) "tiro"
}
string(30) "Cessante causa cessat effectus"
