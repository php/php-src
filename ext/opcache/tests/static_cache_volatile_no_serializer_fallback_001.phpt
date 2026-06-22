--TEST--
OPcache volatile cache: every storable value is encoded as a transparent shared graph (no serializer fallback); values with opaque state are refused
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\VolatileCache as VC;

enum NSFSuit: string { case H = 'h'; }
class NSFPlain { public int $x = 5; }
class NSFSleep { public $a = 1; public $b = 2; function __sleep(): array { return ['a']; } function __wakeup(): void {} }

$i = 0;
function tier($v): string {
    global $i;
    $i++;
    return VC::getInstance('default')->store("nsf_$i", $v) ? 'stored' : 'REFUSED';
}

/* Everything representable as data takes the transparent fast path. */
echo "array:        ", tier(['a' => 1, 'b' => [2, 3]]), "\n";
echo "stdClass:     ", tier((object) ['x' => 1, 'y' => [2]]), "\n";
echo "json object:  ", tier(json_decode('{"a":1,"b":{"c":2}}')), "\n";
echo "plain object: ", tier(new NSFPlain()), "\n";
echo "DateTime:     ", tier(new DateTime('2026-01-01')), "\n";
echo "DateInterval: ", tier(new DateInterval('P1D')), "\n";
echo "ArrayObject:  ", tier(new ArrayObject([1, 2, 3])), "\n";
echo "SplStack:     ", tier((function () { $s = new SplStack(); $s->push(1); return $s; })()), "\n";
echo "SplObjStore:  ", tier((function () { $s = new SplObjectStorage(); $s->attach(new stdClass()); return $s; })()), "\n";
echo "__sleep obj:  ", tier(new NSFSleep()), "\n";
echo "enum:         ", tier(NSFSuit::H), "\n";
$ref = 1;
echo "array w/ ref: ", tier(['x' => &$ref, 'y' => &$ref]), "\n";

/* Values with opaque internal state are refused at store, not stored broken. */
$fiber = new Fiber(function () { Fiber::suspend(); });
$fiber->start();
echo "Fiber:        ", tier($fiber), "\n";
echo "Generator:    ", tier((function () { yield 1; })()), "\n";
echo "WeakMap:      ", tier((function () { $m = new WeakMap(); $m[new stdClass()] = 1; return $m; })()), "\n";

/* Closures are refused at the argument boundary. */
try {
    VC::getInstance('default')->store('nsf_closure', function () {});
    echo "Closure:      stored\n";
} catch (\TypeError $e) {
    echo "Closure:      TypeError\n";
}
?>
--EXPECT--
array:        stored
stdClass:     stored
json object:  stored
plain object: stored
DateTime:     stored
DateInterval: stored
ArrayObject:  stored
SplStack:     stored
SplObjStore:  stored
__sleep obj:  stored
enum:         stored
array w/ ref: stored
Fiber:        REFUSED
Generator:    REFUSED
WeakMap:      REFUSED
Closure:      TypeError
