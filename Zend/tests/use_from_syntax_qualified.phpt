--TEST--
use SomeClass from Namespace with fully-qualified names
--FILE--
<?php

namespace Vendor\Package {
    class Service {
        public static function test() {
            return "Service OK";
        }
    }
}

namespace App {
    class Helper {
        public static function help() {
            return "Helper OK";
        }
    }
}

namespace {
    // Fully-qualified namespace
    use Service from \Vendor\Package;
    echo Service::test() . "\n";

    // Qualified namespace
    use Helper from App;
    echo Helper::help() . "\n";
}

?>
--EXPECT--
Service OK
Helper OK
