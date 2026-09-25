--TEST--
GH-16317 (__debugInfo() overrides don't work)
--EXTENSIONS--
mysqli
--FILE--
<?php

class subclass_mysqli extends mysqli {
    public function __construct() {}
    public function __debugInfo(): array {
        return ['a' => 'b'];
    }
}
var_dump( new subclass_mysqli() );

class subclass_mysqli_result extends mysqli_result {
    public function __construct() {}
    public function __debugInfo(): array {
        return ['o' => 'p'];
    }
}
var_dump( new subclass_mysqli_result() );

class subclass_mysqli_stmt extends mysqli_stmt {
    public function __construct() {}
    public function __debugInfo(): array {
        return ['x' => 'y'];
    }
}
var_dump( new subclass_mysqli_stmt() );

?>
--EXPECT--
object(subclass_mysqli)#1 (1) {
  ["a"]=>
  string(1) "b"
}
object(subclass_mysqli_result)#1 (1) {
  ["o"]=>
  string(1) "p"
}
object(subclass_mysqli_stmt)#1 (1) {
  ["x"]=>
  string(1) "y"
}
