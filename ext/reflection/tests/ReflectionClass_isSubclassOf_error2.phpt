--TEST--
ReflectionClass::isSubclassOf() - fixed crash for unbound anonymous class
--FILE--
<?php
class X {
    public static function main() {
        return new class() extends Base {};
    }
}
class Base {}
$check = function () {
    $base = Base::class;
    foreach (get_declared_classes() as $class) {
        if (strpos($class, '@anonymous') === false) {
            continue;
        }
        echo "Checking for $class\n";
        flush();
        $rc = new ReflectionClass($class);
        var_export($rc->isSubclassOf($base));
        echo "\n";
    }
};
// Should not show up in get_declared_classes until the anonymous class is bound.
$check();
echo "After first check\n";
X::main();
$check();
echo "Done\n";
?>
--EXPECTF--
After first check
Checking for Base@%s
true
Done
