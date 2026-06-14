--TEST--
Bug #69754 (Use of ::class inside array causes compile error)
--FILE--
<?php

class Example {
    public function test() {
        var_dump(static::class);
        var_dump(static::class . 'IsAwesome');
        var_dump(static::class . date('Ymd'));
        var_dump([static::class]);
    }
}

(new Example)->test();

?>
--EXPECTF--
string(7) "Example"
string(16) "ExampleIsAwesome"
string(15) "Example%d"
array(1) {
  [0]=>
  string(7) "Example"
}
