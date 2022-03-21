--TEST--
JIT QM_ASSIGN: 003 missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
    for($cnt2=0;$cnt<6;$cnt++) {
        $a &= 15;
        $a = "4294967295";
        $a *= $a;
        for ($i = 0; $i <= .1; $i++) {
            for ($i = 0; $i << .1; $i++) {
                $a &= $a . $a = "4294967295";
            }
        }
    }
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $cnt in %sqm_assign_003.php on line 3

Warning: Undefined variable $a in %sqm_assign_003.php on line 4

Warning: Undefined variable $cnt in %sqm_assign_003.php on line 3
DONE