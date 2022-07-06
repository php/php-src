--TEST--
Bug #39990 (Cannot "foreach" over overloaded properties)
--FILE--
<?php
  class Foo {
    public function __get($name) {
      return array('Hello', 'World');
    }
  }

  $obj=new Foo();
  foreach($obj->arr as $value)
    echo "$value\n";
?>
--EXPECT--
Hello
World
