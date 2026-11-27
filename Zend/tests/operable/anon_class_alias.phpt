--TEST--
Anonymous Operable class with explicit final operator method works and can be safely aliased
--FILE--
<?php

$a = new readonly class(10) implements Operable {
    public function __construct(public int $v) {}

    final public function __add(int $x): static {
        return new static($this->v + $x);
    }
};

var_dump(($a + 5)->v);

class_alias(get_class($a), 'AliasedAnon');
eval('readonly class SubAnon extends AliasedAnon {}');

$sub = new SubAnon(20);
var_dump(($sub + 7)->v);

$rmAnon = new ReflectionMethod($a, '__add');
$rmSub = new ReflectionMethod(SubAnon::class, '__add');
var_dump($rmAnon->isOperatorMethod(), $rmAnon->isFinal());
var_dump($rmSub->isOperatorMethod(), $rmSub->isFinal());

?>
--EXPECT--
int(15)
int(27)
bool(true)
bool(true)
bool(true)
bool(true)
