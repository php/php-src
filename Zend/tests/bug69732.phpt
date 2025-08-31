--TEST--
Bug #69732 (can induce segmentation fault with basic php code)
--FILE--
<?php
class wpq {
    private $unreferenced;

    public function __get($name) {
        return $this->$name . "XXX";
    }
}

function ret_assoc() {
    $x = "XXX";
    return array('foo' => 'bar', $x);
}

$wpq = new wpq;
$wpq->interesting =& ret_assoc();
$x = $wpq->interesting;
printf("%s\n", $x);
?>
--EXPECTF--
Warning: Undefined property: wpq::$interesting in %s on line %d

Notice: Indirect modification of overloaded property wpq::$interesting has no effect in %sbug69732.php on line 16

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
