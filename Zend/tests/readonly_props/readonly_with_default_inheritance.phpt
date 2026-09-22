--TEST--
Readonly property with default value and inheritance
--FILE--
<?php

class ParentDefault {
    public readonly int $prop = 1;
}

class ChildInherits extends ParentDefault {}

class ChildOverrides extends ParentDefault {
    public readonly int $prop = 2;
}

class PrivateParent {
    private readonly int $prop = 3;

    public function getParentProp(): int {
        return $this->prop;
    }
}

class PrivateChild extends PrivateParent {
    public readonly int $prop = 4;
}

var_dump(new ChildInherits()->prop);
var_dump(new ChildOverrides()->prop);

$privateChild = new PrivateChild();
var_dump($privateChild->getParentProp());
var_dump($privateChild->prop);

?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
