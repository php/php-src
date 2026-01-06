--TEST--
use SomeClass from Namespace - comprehensive test
--FILE--
<?php

// Define classes in different namespaces
namespace Vendor\Package\SubPackage {
    class MyService {
        public static function getName() {
            return "Vendor\\Package\\SubPackage\\MyService";
        }
    }
}

namespace App\Models {
    class User {
        public static function getName() {
            return "App\\Models\\User";
        }
    }
}

namespace Root {
    class Helper {
        public static function getName() {
            return "Root\\Helper";
        }
    }
}

// Use the new "from" syntax
namespace {
    // Import with qualified namespace
    use MyService from Vendor\Package\SubPackage;
    echo MyService::getName() . "\n";

    // Import with fully-qualified namespace
    use User from \App\Models;
    echo User::getName() . "\n";

    // Import from simple namespace
    use Helper from Root;
    echo Helper::getName() . "\n";

    // Verify classes work (the full class names exist, not the aliases)
    echo class_exists('Vendor\\Package\\SubPackage\\MyService') ? "Full class name works\n" : "FAIL\n";
    echo class_exists('App\\Models\\User') ? "Full class name works\n" : "FAIL\n";
    echo class_exists('Root\\Helper') ? "Full class name works\n" : "FAIL\n";
}

?>
--EXPECT--
Vendor\Package\SubPackage\MyService
App\Models\User
Root\Helper
Full class name works
Full class name works
Full class name works
