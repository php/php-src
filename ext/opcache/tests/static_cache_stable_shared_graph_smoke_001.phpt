--TEST--
OPcache stable cache: shared graph smoke coverage for references, enums, hooked objects, and opaque-state refusal
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.stable_size_mb=32
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\StableCache;

enum StableSmokeSuit: string
{
    case Hearts = 'H';
}

class StableSmokeHooked
{
    public static int $serializeCalls = 0;
    public static int $unserializeCalls = 0;

    public function __construct(public int $value = 0) {}

    public function __serialize(): array
    {
        self::$serializeCalls++;

        return ['value' => $this->value];
    }

    public function __unserialize(array $data): void
    {
        self::$unserializeCalls++;
        $this->value = $data['value'];
    }
}

class StableSmokeBox
{
    public ?StableSmokeHooked $left = null;
    public ?StableSmokeHooked $right = null;
}

$cache = StableCache::getInstance('shared_graph_smoke');
$cache->clear();

$ref = 1;
$refs = ['value' => &$ref, 'alias' => &$ref];
var_dump($cache->store('refs', $refs));
$fetchedRefs = $cache->fetch('refs');
$fetchedRefs['value'] = 42;
echo "reference alias: ";
var_dump($fetchedRefs['alias'] === 42);

var_dump($cache->store('enum', ['case' => StableSmokeSuit::Hearts]));
echo "enum singleton: ";
var_dump($cache->fetch('enum')['case'] === StableSmokeSuit::Hearts);

$hooked = new StableSmokeHooked(7);
$box = new StableSmokeBox();
$box->left = $hooked;
$box->right = $hooked;
StableSmokeHooked::$serializeCalls = 0;
StableSmokeHooked::$unserializeCalls = 0;
var_dump($cache->store('hooked', $box));
echo "serialize calls: ", StableSmokeHooked::$serializeCalls, "\n";

$first = $cache->fetch('hooked');
$second = $cache->fetch('hooked');
echo "hooked identity: ";
var_dump($first->left === $first->right);
echo "hooked fetches independent: ";
var_dump($first->left !== $second->left);
echo "hooked value: ", $first->left->value, "\n";
echo "unserialize calls: ", StableSmokeHooked::$unserializeCalls, "\n";

echo "generator refused: ";
var_dump($cache->store('opaque_value', (function () { yield 1; })()));
echo "generator miss: ";
var_dump($cache->fetch('opaque_value'));
?>
--EXPECT--
bool(true)
reference alias: bool(true)
bool(true)
enum singleton: bool(true)
bool(true)
serialize calls: 1
hooked identity: bool(true)
hooked fetches independent: bool(true)
hooked value: 7
unserialize calls: 1
generator refused: bool(false)
generator miss: NULL
