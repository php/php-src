--TEST--
Test extract() function - ensure EXTR_OBJECT|EXTR_PREFIX works
--FILE--
<?php
class test {
    public $hello = "exists";
    
    public function __construct(&$server) {
        extract($server, EXTR_OBJECT|EXTR_PREFIX_SAME, "testing", $this);
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
  string(6) "exists"
  ["testing_hello"]=>
  string(5) "world"
}


