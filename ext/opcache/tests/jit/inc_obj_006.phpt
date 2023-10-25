--TEST--
PRE_INC_OBJ: 006
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
$a = new class {
    public int $foo = 0;

    function &__get($x) {
        return $this->foo;
    }

    function __set($x, $y) {
        echo "set($y)\n";
    }
};

--$a->x->y;
?>
DONE
--EXPECTF--
Fatal error: Uncaught Error: Attempt to increment/decrement property "y" on int in %sinc_obj_006.php:14
Stack trace:
#0 {main}
  thrown in %sinc_obj_006.php on line 14

