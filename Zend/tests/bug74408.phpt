--TEST--
Bug #74408 (Endless loop bypassing execution time limit)
--INI--
error_reporting = E_ALL | E_DEPRECATED | E_STRICT
--FILE--
<?php

 //php.ini: error_reporting = E_ALL | E_DEPRECATED | E_STRICT

 class ErrorHandling {

  public  function error_handler($errno, $errstr, $errfile, $errline) {
	  $bla = new NonExistingClass2();
  }

  public function exception_handler(Error $e) {
	  echo "Caught, exception: " . $e->getMessage();
  }
 }

 set_error_handler('ErrorHandling::error_handler');
 set_exception_handler('ErrorHandling::exception_handler');

 $blubb = new NonExistingClass();
?>
--EXPECTF--
Deprecated: Non-static method ErrorHandling::error_handler() should not be called statically in %sbug74408.php on line %d

Deprecated: Non-static method ErrorHandling::error_handler() should not be called statically in %sbug74408.php on line %d

Deprecated: Non-static method ErrorHandling::error_handler() should not be called statically in Unknown on line 0

Fatal error: Uncaught Error: Class 'NonExistingClass2' not found in %sbug74408.php:%d
Stack trace:
#0 [internal function]: ErrorHandling::error_handler(8192, 'Non-static meth...', '%s', %d, Array)
#1 %sbug74408.php(%d): set_exception_handler('ErrorHandling::...')
#2 {main}
  thrown in %sbug74408.php on line %d
