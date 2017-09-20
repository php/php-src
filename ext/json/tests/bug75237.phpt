--TEST--
Bug #75237 (jsonSerialize() - Returning new instance of self causes segfault)
--SKIPIF--
<?php if (!extension_loaded("json")) die("skip ext/json required"); ?>
--FILE--
<?php
class Foo implements JsonSerializable {
  public function jsonSerialize() {
    return new self;
  }
}

try {
  var_dump(json_encode(new Foo));
} catch (Exception $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
Foo::jsonSerialize() cannot return a new instance of itself
