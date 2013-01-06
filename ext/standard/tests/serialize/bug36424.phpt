--TEST--
Bug #36424 - Serializable interface breaks object references
--FILE--
<?php

echo "-TEST\n";

class a implements Serializable {
    function serialize() {
        return serialize(get_object_vars($this));
    }
    function unserialize($s) {
        foreach (unserialize($s) as $p=>$v) {
            $this->$p=$v;
        }
    }
}
class b extends a {}
class c extends b {}

$c = new c;
$c->a = new a;
$c->a->b = new b;
$c->a->b->c = $c;
$c->a->c = $c;
$c->a->b->a = $c->a;
$c->a->a = $c->a;

$s = serialize($c);
printf("%s\n", $s);

$d = unserialize($s);

var_dump(
    $d === $d->a->b->c,
    $d->a->a === $d->a,
    $d->a->b->a === $d->a,
    $d->a->c === $d
);

print_r($d);

echo "Done\n";

?>
--EXPECTF--
%aTEST
C:1:"c":108:{a:1:{s:1:"a";C:1:"a":81:{a:3:{s:1:"b";C:1:"b":30:{a:2:{s:1:"c";r:1;s:1:"a";r:3;}}s:1:"c";r:1;s:1:"a";r:3;}}}}
bool(true)
bool(true)
bool(true)
bool(true)
c Object
(
    [a] => a Object
        (
            [b] => b Object
                (
                    [c] => c Object
 *RECURSION*
                    [a] => a Object
 *RECURSION*
                )

            [c] => c Object
 *RECURSION*
            [a] => a Object
 *RECURSION*
        )

)
Done
