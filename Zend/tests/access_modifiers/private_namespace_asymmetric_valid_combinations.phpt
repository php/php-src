--TEST--
Valid asymmetric visibility combinations with private(namespace)
--FILE--
<?php

namespace Test;

class A {
    // Valid: public base allows any set visibility
    public private(namespace)(set) string $prop1 = "test1";
    public protected(set) string $prop2 = "test2";
    public private(set) string $prop3 = "test3";

    // Valid: protected base allows protected(set) or private(set) (inheritance axis only)
    protected protected(set) string $prop4 = "test4";
    protected private(set) string $prop5 = "test5";

    // Valid: private(namespace) base allows only private(namespace)(set) (namespace axis only)
    private(namespace) private(namespace)(set) string $prop6 = "test6";

    // Valid: private base allows only private(set)
    private private(set) string $prop7 = "test7";

    public function test() {
        echo "prop1: " . $this->prop1 . "\n";
        echo "prop2: " . $this->prop2 . "\n";
        echo "prop3: " . $this->prop3 . "\n";
        echo "prop4: " . $this->prop4 . "\n";
        echo "prop5: " . $this->prop5 . "\n";
        echo "prop6: " . $this->prop6 . "\n";
        echo "prop7: " . $this->prop7 . "\n";
    }
}

$a = new A();
$a->test();

echo "All valid combinations work correctly!\n";

?>
--EXPECT--
prop1: test1
prop2: test2
prop3: test3
prop4: test4
prop5: test5
prop6: test6
prop7: test7
All valid combinations work correctly!
