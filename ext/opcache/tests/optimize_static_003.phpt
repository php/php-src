--TEST--
Keep BIND_STATIC when static variable has an initializer
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function bar() {
    return 42;
}
function foo() {
    static $a = bar();
    var_dump((new ReflectionFunction(__FUNCTION__))->getStaticVariables());
}
foo();
?>
--EXPECT--
array(1) {
  ["a"]=>
  &int(42)
}
