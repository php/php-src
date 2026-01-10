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
function foo() {
    // Eliminating this does have a small side effect of not showing & for $a when var_dumping static variables
    static $a = 42;
    var_dump((new ReflectionFunction(__FUNCTION__))->getStaticVariables());
}
foo();
?>
--EXPECT--
array(1) {
  ["a"]=>
  int(42)
}
