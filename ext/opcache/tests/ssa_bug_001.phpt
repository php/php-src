--TEST--
SSA construction for CFG with unreachable basic blocks
--EXTENSIONS--
opcache
--FILE--
<?php
class X {
    public function __get($n) {
      if ($n === 'type') {
        trigger_error('Deprecated type property called; use instanceof', E_USER_NOTICE);
        switch (get_class($this)) {
          case 'HTMLPurifier_Token_Start':      return 'start';
          default: return null;
        }
      }
    }
}
?>
OK
--EXPECT--
OK
