--TEST--
GH-21691 (OPcache CFG optimizer breaks reference returns with JMPZ/JMPZ_EX)
--INI--
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
class Base {
    protected function &getData(): array {
        $x = [];
        return $x;
    }

    public function process(): array {
        if ($data = &$this->getData() && !isset($data['key'])) {
        }
        return $data;
    }

    public function check(): bool {
        return ($data = &$this->getData()) && count($data) > 0;
    }
}

class Child extends Base {
    protected function &getData(): array {
        static $x = ['value' => 42];
        return $x;
    }
}

$child = new Child();
var_dump($child->process());
var_dump($child->check());
?>
--EXPECT--
array(1) {
  ["value"]=>
  int(42)
}
bool(true)
