--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): array variation
--FILE--
<?php

class Test
{
    static $instances;
    public function __construct() {
	    var_dump(self::$instances[NULL] = $this);
	    var_dump(self::$instances);
    }

    function __destruct() {
        unset(self::$instances[NULL]);
    }
}
new Test();
new Test();

?>
--EXPECT--
object(Test)#1 (0) {
}
array(1) {
  [""]=>
  object(Test)#1 (0) {
  }
}
object(Test)#2 (0) {
}
array(0) {
}
