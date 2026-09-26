--TEST--
JIT FETCH_OBJ: Typed property that becomes a reference or undefined
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    public int $x = 1;
    public float $f = 1.5;

    function becomes_ref() {
        $s = 0;
        for ($i = 0; $i < 300; $i++) {
            $s += $this->x;
            if ($i == 150) {
                $r = &$this->x;
                $r = 2;
            }
        }
        return $s;
    }

    function becomes_ref_float() {
        $s = 0.0;
        for ($i = 0; $i < 300; $i++) {
            $s += $this->f;
            if ($i == 150) {
                $r = &$this->f;
                $r = 2.5;
            }
        }
        return $s;
    }

    function unset_isset() {
        $s = 0;
        for ($i = 0; $i < 300; $i++) {
            $s += $this->x ?? 7;
            if ($i == 150) unset($this->x);
        }
        return $s;
    }
}

class M {
    public int $x = 1;

    function __get($name) {
        return 1000;
    }

    function unset_get() {
        $s = 0;
        for ($i = 0; $i < 300; $i++) {
            $s += $this->x;
            if ($i == 150) unset($this->x);
        }
        return $s;
    }
}

var_dump((new A)->becomes_ref());
var_dump((new A)->becomes_ref_float());
var_dump((new A)->unset_isset());
var_dump((new M)->unset_get());

?>
--EXPECT--
int(449)
float(599)
int(1194)
int(149151)
