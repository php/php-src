--TEST--
Bug #72306 (Heap overflow through proc_open and $env parameter)
--FILE--
<?php
class moo {
    function __construct() { $this->a = 0; }
    function __toString() { return $this->a++ ? str_repeat("a", 0x8000) : "a"; }
}

$env = array('some_option' => new moo());
$pipes = array();
$description = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
   2 => array("pipe", "r")
);

$process = proc_open('nothing', $description, $pipes, NULL, $env);

?>
okey
--EXPECT--
okey
