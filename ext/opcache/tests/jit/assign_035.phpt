--TEST--
JIT ASSIGN: Segfault & memleak if no RC info
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php

class A {

  public function test() {
    $closure = function() { return "string"; };

    $arr = [
      'a' => $closure(),
      'b' => [$closure() => [],],
     ];

    $x = $arr;
    unset($x['b'][$closure()]['d']);

    $x = $arr;
    $x['a'] = $closure();

    return "okey";
  }
}

$a = new A();
echo $a->test();
?>
--EXPECT--
okey
