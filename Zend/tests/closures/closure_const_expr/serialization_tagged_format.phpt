--TEST--
Closure serialization payload is a tagged union of [class, id] references
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

// The payload is [ <object properties>, [ <tag>, [class, id] ] ]. The
// properties slot is empty, the tag is "const-expr". An anonymous closure's
// id embeds a code hash ("<site>@<rank>#<hash>"); a first-class callable's
// id is its name key, which cannot drift and carries none.
$anonPayload = $anon->__serialize();
$fccPayload = $fcc->__serialize();
var_dump($anonPayload[0], $anonPayload[1][0]);
var_dump(count($anonPayload[1][1]), count($fccPayload[1][1]));
var_dump($anonPayload[1][1][0]);
var_dump(preg_match('/^\$p@0#[0-9a-f]{8}$/', $anonPayload[1][1][1]));
var_dump($fccPayload[1][1][1]);

?>
--EXPECT--
array(0) {
}
string(10) "const-expr"
int(2)
int(2)
string(4) "Demo"
int(1)
string(9) "$p@strlen"
