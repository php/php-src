--TEST--
JIT ASSIGN_OP: 009 incorrect guard motion out of the loop
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
	$x = $a = $c = 0;
    for($cnt=0;$cnt<6;$cnt++) {
        $a *= $a;
        for ($i = 0; $i <= .1; !$j++)
            for ($i = 0; $i <= .1; !$i++)
                for ($i = 0; $i << .1; !$i++);
        $x != $a ?: $c;
        $a = "3566715245541";
    }
}
@foo();
@foo();
@foo();
?>
DONE
--EXPECT--
DONE
