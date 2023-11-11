--TEST--
JIT ASSIGN: 027
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
  $persons = 2;
  for ($i=0; $i<$persons; $i++) {
    $children = 2;
  }
}
foo();
echo "ok\n";
?>
--EXPECT--
ok
