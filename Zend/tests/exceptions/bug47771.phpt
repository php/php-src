--TEST--
Bug #47771 (Exception during object construction from arg call calls object's destructor)
--FILE--
<?php
function throw_exc() {
  throw new Exception('TEST_EXCEPTION');
}

class Test {

  public function __construct() {
    echo 'Constr' ."\n";
  }

  public function __destruct() {
    echo 'Destr' ."\n";
  }

}

try {

  $T =new Test(throw_exc());

} catch( Throwable $e) {
  echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: TEST_EXCEPTION
