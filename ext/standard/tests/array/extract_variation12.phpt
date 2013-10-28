--TEST--
Test extract() function - ensure EXTR_OBJECT works
--FILE--
<?php
class test {
    public function __construct(&$server) {
        extract($server, EXTR_OBJECT, $this);
    }
}

$array = array(
    "hello" => "world"
);

var_dump(new test($array));
?>
--EXPECTF--
object(test)#%d (%d) {
  ["hello"]=>
  string(5) "world"
}

