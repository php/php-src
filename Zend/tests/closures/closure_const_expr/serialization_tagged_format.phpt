--TEST--
Closure serialization payload is a tagged union of [class, site, key, hash] references
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    #[A(static function () { return 'anon'; })]
    #[A(strlen(...))]
    public int $p = 0;
}

$attrs = (new ReflectionProperty(Demo::class, 'p'))->getAttributes();
$anon = $attrs[0]->getArguments()[0];
$fcc = $attrs[1]->getArguments()[0];

// The payload is [ <object properties>, [ <tag>, [class, site, key, hash] ] ].
// The properties slot is empty, the tag is "const-expr". An anonymous closure's
// key is an int rank with a non-zero code hash; a first-class callable's key is
// its name, which cannot drift and carries a zero hash.
$anonPayload = $anon->__serialize();
$fccPayload = $fcc->__serialize();
var_dump($anonPayload[0], $anonPayload[1][0]);
var_dump(count($anonPayload[1][1]), count($fccPayload[1][1]));

// anonymous: [class, site, rank, hash]
var_dump($anonPayload[1][1][0], $anonPayload[1][1][1], $anonPayload[1][1][2]);
var_dump($anonPayload[1][1][3] !== 0);

// first-class callable: [class, site, name, 0]
var_dump($fccPayload[1][1][1], $fccPayload[1][1][2], $fccPayload[1][1][3]);

?>
--EXPECT--
array(0) {
}
string(10) "const-expr"
int(4)
int(4)
string(4) "Demo"
string(2) "$p"
int(0)
bool(true)
string(2) "$p"
string(6) "strlen"
int(0)
