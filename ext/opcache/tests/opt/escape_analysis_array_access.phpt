--TEST--
Escape analysis: objects implementing ArrayAccess escape through dimension accesses
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
#[AllowDynamicProperties]
class AA implements ArrayAccess {
    function offsetExists($o): bool { $this->x = 3; return true; }
    function offsetGet($o): mixed { $this->x = 2; return 42; }
    function offsetSet($o, $v): void { $this->x = 4; }
    function offsetUnset($o): void {}

    static function fetch() {
        $o = new self;
        $o->x = 1;
        $o[0];
        return $o->x;
    }

    static function isset() {
        $o = new self;
        $o->x = 1;
        return [isset($o[0]), $o->x];
    }

    static function assign() {
        $o = new self;
        $o->x = 1;
        $o[0] = 1;
        return $o->x;
    }
}

#[AllowDynamicProperties] class B {}
#[AllowDynamicProperties]
class BB implements ArrayAccess {
    function offsetExists($o): bool {
        return true;
    }

    function offsetGet($o): mixed {
        $this->x = 2;
        return 1;
    }

    function offsetSet($o, $v): void {}

    function offsetUnset($o): void {}

    static function f($c) {
        if ($c) { $o = new B; $o->x = 1; } else { $o = new self; $o->x = 1; }
        $o[0];
        return $o->x;
    }
}

var_dump(AA::fetch());
var_dump(AA::isset());
var_dump(AA::assign());
var_dump(BB::f(false));
?>
--EXPECT--
int(2)
array(2) {
  [0]=>
  bool(true)
  [1]=>
  int(3)
}
int(4)
int(2)
