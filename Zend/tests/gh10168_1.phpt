--TEST--
GH-10168 (heap-buffer-overflow at zval_undefined_cv): array variation
--FILE--
<?php

class Test
{
    static $instances;
    public function __construct() {
	    (self::$instances[NULL] = $this) > 0;
	    var_dump(self::$instances);
    }

    function __destruct() {
        unset(self::$instances[NULL]);
    }
}
new Test();
new Test();

?>
--EXPECTF--
Notice: Object of class Test could not be converted to int in %s on line %d
array(1) {
  [""]=>
  object(Test)#1 (0) {
  }
}

Notice: Object of class Test could not be converted to int in %s on line %d
array(0) {
}
