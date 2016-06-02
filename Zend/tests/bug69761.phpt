--TEST--
Bug #69761 (Serialization of anonymous classes should be prevented)
--FILE--
<?php
$instance = new class('foo') {
    public function __construct($i) {
    }
};
var_dump(serialize($instance));
?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'class@%s' is not allowed in %sbug69761.php:%d
Stack trace:
#0 %sbug69761.php(%d): serialize(Object(class@anonymous))
#1 {main}
  thrown in %sbug69761.php on line %d
