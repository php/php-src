--TEST--
The closure name includes the source location (2)
--FILE--
<?php

namespace NameSpaceName {
    class ClassName {
        public function methodName() {
            $c = function () {};
            $r = new \ReflectionFunction($c);
            var_dump($r->name);
        }

        public function nestedClosure() {
            $c = function () {
                $c = function () {
                    $c = function () {};
                    $r = new \ReflectionFunction($c);
                    var_dump($r->name);
                };

                $c();
            };

            $c();
        }
    }

    function function_name() {
        $c = function () { };
        $r = new \ReflectionFunction($c);
        var_dump($r->name);
    }
}

namespace {
    class ClassName {
        public function methodName() {
            $c = function () {};
            $r = new \ReflectionFunction($c);
            var_dump($r->name);
        }

        public function nestedClosure() {
            $c = function () {
                $c = function () {
                    $c = function () {};
                    $r = new \ReflectionFunction($c);
                    var_dump($r->name);
                };

                $c();
            };

            $c();
        }
    }

    function function_name() {
        $c = function () { };
        $r = new \ReflectionFunction($c);
        var_dump($r->name);
    }

    $class = new \NameSpaceName\ClassName();
    $class->methodName();
    $class->nestedClosure();
    \NameSpaceName\function_name();

    $class = new \ClassName();
    $class->methodName();
    $class->nestedClosure();
    \function_name();

    $c = function () { };
    $r = new \ReflectionFunction($c);
    var_dump($r->name);
}

?>
--EXPECTF--
string(49) "{closure:NameSpaceName\ClassName::methodName():6}"
string(79) "{closure:{closure:{closure:NameSpaceName\ClassName::nestedClosure():12}:13}:14}"
string(42) "{closure:NameSpaceName\function_name():27}"
string(36) "{closure:ClassName::methodName():36}"
string(65) "{closure:{closure:{closure:ClassName::nestedClosure():42}:43}:44}"
string(28) "{closure:function_name():57}"
string(%d) "{closure:%sclosure_%d.php:72}"
