--TEST--
get_object_vars() fast/slow-path discrepancies
--EXTENSIONS--
json
--FILE--
<?php

$obj = (object)[
    "\0A\0b" => 42,
    "\0*\0c" => 24,
    12 => 6,
];
$obj->test = new class implements JsonSerializable {
    public function jsonSerialize(): mixed {
        var_dump(get_object_vars($GLOBALS['obj']));
        return null;
    }
};

var_dump(get_object_vars($obj));

// Use json_encode to get a dump with apply_count > 0
json_encode($obj);

?>
--EXPECTF--
array(4) {
  ["%0A%0b"]=>
  int(42)
  ["%0*%0c"]=>
  int(24)
  [12]=>
  int(6)
  ["test"]=>
  object(class@anonymous)#2 (0) {
  }
}
array(4) {
  ["%0A%0b"]=>
  int(42)
  ["%0*%0c"]=>
  int(24)
  [12]=>
  int(6)
  ["test"]=>
  object(class@anonymous)#2 (0) {
  }
}
