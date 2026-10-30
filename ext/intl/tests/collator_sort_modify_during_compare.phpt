--TEST--
Collator::sort(): mutating the array from __toString() during comparison must not corrupt the sort
--EXTENSIONS--
intl
--FILE--
<?php
$c = new Collator('en_US');

class Grow {
    public static array $ref;
    public function __toString(): string {
        for ($i = 0; $i < 2000; $i++) {
            self::$ref[] = "x$i";
        }
        return "m";
    }
}

$arr = ["z", new Grow(), "a", "b"];
Grow::$ref = &$arr;
var_dump($c->sort($arr));
var_dump($arr);
?>
--EXPECT--
bool(true)
array(4) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  object(Grow)#2 (0) {
  }
  [3]=>
  string(1) "z"
}
