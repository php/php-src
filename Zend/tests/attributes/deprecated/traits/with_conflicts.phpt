--TEST--
#[\Deprecated]: Using multiple traits with conflict resolution
--FILE--
<?php

#[Deprecated]
trait DemoTraitA {
    public function lowerCase(): string {
        return 'a';
    }
    public function upperCase(): string {
        return 'A';
    }
}

#[Deprecated]
trait DemoTraitB {
    public function lowerCase(): string {
        return 'b';
    }
    public function upperCase(): string {
        return 'B';
    }
}

class DemoClass {
    use DemoTraita, DemoTraitB {
        DemoTraitA::lowerCase insteadof DemoTraitB;
        DemoTraitB::upperCase insteadof DemoTraitA;
    }
}

$d = new DemoClass();
var_dump($d->lowerCase());
var_dump($d->upperCase());

?>
--EXPECTF--
Deprecated: Trait DemoTraitA used by DemoClass is deprecated in %s on line %d

Deprecated: Trait DemoTraitB used by DemoClass is deprecated in %s on line %d
string(1) "a"
string(1) "B"
