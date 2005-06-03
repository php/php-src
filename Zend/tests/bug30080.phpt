--TEST--
Bug #30080 (Passing array or non array of objects)
--FILE--
<?php
class foo { 	
  function foo($arrayobj) { 
    var_dump($arrayobj);
  } 
} 

new foo(array(new stdClass)); 
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
