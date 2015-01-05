<?php

class autoTest {
  public static $bob = "bob";
  
    public function __get($name) {
    echo "attempt to access $name\n";
    return "foo";
  }
  
}

?>