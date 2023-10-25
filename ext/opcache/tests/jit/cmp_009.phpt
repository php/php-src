--TEST--
JIT CMP: 009 Wrong code generation for dead compare
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 0; $i < 20; $j++) {
    $a&=
    $a = $a.+
    $tojenthiss[$i = $v] = $s=
    $tojenthiss[$i = $v] = $s=
    $a&=
    $b =
    $b =
    $a = $a.+
$obj->prop =
    $a = $a.+
$obj->prop =
    $a = $a.+
    $Sojenthiss[$i = $v] = $s=
    $x!= $a ?: $c;
    $x!= $a ?: $c;
    $x!= $a ?: $c;
    $x!= $a ?: $c;
    $aZ = $a;
    $aZ = $a;
    $aZ = $a;
    $a = $a.+
    $a = $a.+
    $x!= $a ?: $c;
    $aZ = $a;
    $a = $a.+
    $a = $a.+
$obj->prop =
    $a = $a.+
    $Sojenthiss[$i = $v] = $s=
    $x!= $a ?: $c;
    $x!= $a ?: $c;
    $aZ = $a;
    }
}
@test();
?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to assign property "prop" on null in %scmp_009.php:15
Stack trace:
#0 %scmp_009.php(39): test()
#1 {main}
  thrown in %scmp_009.php on line 15
