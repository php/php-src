--TEST--
Namespaced constant lookup is fully case-sensitive
--FILE--
<?php
namespace MyApp {
    const GREETING = 'hello';
}

namespace {
    define('MyApp\DEFINED_CONST', 'defined');

    var_dump(\MyApp\GREETING);
    var_dump(MyApp\DEFINED_CONST);
    var_dump(constant('MyApp\GREETING'));
    var_dump(defined('MyApp\GREETING'));

    // Wrong-case namespace segment is not found.
    var_dump(defined('myapp\GREETING'));
    var_dump(constant('MYAPP\DEFINED_CONST'));
}
?>
--EXPECTF--
string(5) "hello"
string(7) "defined"
string(5) "hello"
bool(true)
bool(false)

Fatal error: Uncaught Error: Undefined constant "MYAPP\DEFINED_CONST" in %s:%d
Stack trace:
#0 %s(%d): constant('MYAPP\\DEFINED_C...')
#1 {main}
  thrown in %s on line %d
