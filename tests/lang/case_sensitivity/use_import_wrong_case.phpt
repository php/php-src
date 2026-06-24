--TEST--
File-level use import with wrong-cased namespace path fails with wrong case
--FILE--
<?php
namespace MyApp\Service;

class UserService {
    public function name(): string { return "UserService"; }
}

namespace Test\Wrong;

use myapp\service\UserService;

$obj = new UserService();
echo get_class($obj) . "\n";

namespace Test\AlsoWrong;

use MYAPP\SERVICE\UserService as US;

$obj2 = new US();
echo get_class($obj2) . "\n";

namespace Test\Correct;

use MyApp\Service\UserService as Correct;

$obj3 = new Correct();
echo get_class($obj3) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "myapp\service\UserService" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
