--TEST--
Make sure trait does not implement an interface.
--FILE--
<?php
error_reporting(E_ALL);

interface MyInterface {
    public function a();
}

trait THello implements MyInterface {
  public function a() {
    echo 'A';
  }
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "implements", expecting "{" in %s on line %d
