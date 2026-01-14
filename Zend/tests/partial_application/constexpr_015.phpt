--TEST--
PFA in constexpr: preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/constexpr_015.inc
--FILE--
<?php

echo "# Class const\n";
var_dump((C::CC)('hello'));
echo "# Class attr\n";
var_dump((new ReflectionClass(C::class)->getAttributes(A::class)[0]->newInstance()->fn)('hello'));
echo "# Method attr\n";
var_dump((new ReflectionMethod(C::class, 'f')->getAttributes(A::class)[0]->newInstance()->fn)('hello'));
echo "# Method default value\n";
var_dump((new C()->f())('hello'));

?>
--EXPECT--
# Class const
int(5)
# Class attr
int(5)
# Method attr
int(5)
# Method default value
int(5)
