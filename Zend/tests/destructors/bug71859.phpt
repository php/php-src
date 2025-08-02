--TEST--
Bug #71859 (zend_objects_store_call_destructors operates on realloced memory, crashing)
--FILE--
<?php
class constructs_in_destructor {
  public $a;
  public function __destruct() {
    //We are now in zend_objects_store_call_destructors
    //This causes a realloc in zend_objects_store_put
    for ($i = 0; $i < 10000; ++$i) {
      $GLOBALS["a$i"] = new stdClass;
    }
    //Returns to zend_objects_store_call_destructors, to access freed memory.
  }
}

$a = new constructs_in_destructor;
//Create cycle so destructors are ran only in zend_objects_store_call_destructors
$a->a = $a;

// Create some objects so zend_objects_store_call_destructors has something
// to do after constructs_in_destructor is destroyed.
for ($i = 0; $i < 200; ++$i) {
  $GLOBALS["b$i"] = new stdClass;
}
?>
okey
--EXPECT--
okey
