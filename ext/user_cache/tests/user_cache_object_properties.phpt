--TEST--
UserCache\Cache: uninitialized typed, missing-scope private and empty-named properties decode correctly, and malformed mangled property names from a state are not reported at store() and fail the restore like unserialize()
--INI--
user_cache.enable=1
user_cache.enable_cli=1
user_cache.shm_size=16M
--FILE--
<?php
function ok(string $label, bool $cond): void
{
    echo $label, ': ', $cond ? 'OK' : 'FAIL', "\n";
}

/* Uninitialized typed properties stay uninitialized at the root and nested */
$cache = UserCache\Cache::getPool('object-props-uninitialized');

class UninitBox
{
    public int $assigned = 0;
    public int $neverAssigned;
    public ?string $nullableNeverAssigned;
    public string $withDefault = 'default';
}

class UninitReadonly
{
    public readonly int $id;
    public string $name;
    public function __construct(string $name)
    {
        $this->name = $name;
    }
}

$object = new UninitBox();
$object->assigned = 42;

$cache->store('box', $object);
$fetched = $cache->fetch('box');

$assignedProp = new ReflectionProperty($fetched, 'assigned');
$neverProp = new ReflectionProperty($fetched, 'neverAssigned');
$nullableProp = new ReflectionProperty($fetched, 'nullableNeverAssigned');
$defaultProp = new ReflectionProperty($fetched, 'withDefault');

ok('assigned initialized', $assignedProp->isInitialized($fetched) && $fetched->assigned === 42);
ok('never-assigned stays uninitialized', !$neverProp->isInitialized($fetched));
ok('nullable-never stays uninitialized', !$nullableProp->isInitialized($fetched));
ok('default initialized', $defaultProp->isInitialized($fetched) && $fetched->withDefault === 'default');

$readError = false;
try {
    $fetched->neverAssigned;
} catch (Error $e) {
    $readError = str_contains($e->getMessage(), 'must not be accessed before initialization');
}
ok('uninitialized read still errors', $readError);

$fetched->neverAssigned = 7;
ok('can assign after fetch', $fetched->neverAssigned === 7);
$typeError = false;
try {
    $fetched->assigned = 'not an int';
} catch (TypeError $e) {
    $typeError = true;
}
ok('type check preserved', $typeError);

$ro = new UninitReadonly('n');
$cache->store('ro', $ro);
$fetchedRo = $cache->fetch('ro');
$idProp = new ReflectionProperty($fetchedRo, 'id');
ok('readonly uninitialized survives', !$idProp->isInitialized($fetchedRo) && $fetchedRo->name === 'n');

$graph = ['direct' => new UninitBox(), 'list' => [new UninitBox()]];
$graph['direct']->assigned = 1;
$cache->store('graph', $graph);
$fetchedGraph = $cache->fetch('graph');
$nestedProp = new ReflectionProperty($fetchedGraph['list'][0], 'neverAssigned');
ok('nested uninitialized survives', !$nestedProp->isInitialized($fetchedGraph['list'][0]));
ok('serialize parity', serialize($cache->fetch('box')) === serialize($object));

/* A private property of a missing declaring class round-trips like unserialize() */
$cache = UserCache\Cache::getPool('object-props-missing-scope');
$cache->clear();

#[AllowDynamicProperties]
class UserCacheMissingScopeBox
{
}

$payload = 'O:24:"UserCacheMissingScopeBox":1:{s:15:"' . "\0Missing\0secret" . '";i:42;}';
$value = unserialize($payload);

var_dump($cache->store('value', $value));
var_dump($cache->fetch('value', 'default'));
var_dump(serialize($cache->fetch('value')) === $payload);

/* An empty property name decodes on the first and the slot-cached fetch */
$cache = UserCache\Cache::getPool('object-props-empty-name');
$cache->clear();

#[AllowDynamicProperties]
class UserCacheEmptyPropertyNameBox
{
    public int $declared = 1;
}

$plain = (object) ['' => 'empty-name'];
var_dump($cache->store('plain', $plain));
var_dump($cache->fetch('plain', 'default'));
var_dump($cache->fetch('plain', 'default'));

$box = new UserCacheEmptyPropertyNameBox();
$box->{''} = 'empty-name';
var_dump($cache->store('box', $box));
var_dump($cache->fetch('box', 'default'));

class Malformed
{
    public $a = 1;

    public function __serialize(): array
    {
        return ["\0x" => 1, "\0A\0" => 2, 'a' => 5];
    }
}

function malformed_property_names(): void
{
    $notices = [];
    set_error_handler(function (int $errno, string $message) use (&$notices): bool {
        $notices[] = $message;

        return true;
    });

    $cache = UserCache\Cache::getPool('malformed-property-names');
    var_dump($cache->store('k', new Malformed));
    echo 'store notices: ', count($notices), "\n";

    $notices = [];
    var_dump($cache->fetch('k', 'DEFAULT'));
    echo 'fetch notices: ', implode(' | ', array_unique($notices)), "\n";

    $notices = [];
    var_dump(unserialize(serialize(new Malformed)));
    echo 'unserialize notices: ', implode(' | ', $notices), "\n";

    /* Classes without declared properties keep such names as dynamic properties, like unserialize(). */
    $notices = [];
    $undeclared = (object) ["\0x" => 1, 'ok' => 2];
    var_dump($cache->store('undeclared', $undeclared));
    var_dump($cache->fetch('undeclared') == unserialize(serialize($undeclared)), $cache->fetch('undeclared') == $undeclared);
    echo 'undeclared notices: ', count($notices), "\n";

    restore_error_handler();
}

echo "\nmalformed property names:\n";
malformed_property_names();
?>
--EXPECTF--
assigned initialized: OK
never-assigned stays uninitialized: OK
nullable-never stays uninitialized: OK
default initialized: OK
uninitialized read still errors: OK
can assign after fetch: OK
type check preserved: OK
readonly uninitialized survives: OK
nested uninitialized survives: OK
serialize parity: OK
bool(true)
object(UserCacheMissingScopeBox)#%d (1) {
  ["secret":"Missing":private]=>
  int(42)
}
bool(true)
bool(true)
object(stdClass)#%d (1) {
  [""]=>
  string(10) "empty-name"
}
object(stdClass)#%d (1) {
  [""]=>
  string(10) "empty-name"
}
bool(true)
object(UserCacheEmptyPropertyNameBox)#%d (2) {
  ["declared"]=>
  int(1)
  [""]=>
  string(10) "empty-name"
}

malformed property names:
bool(true)
store notices: 0
string(7) "DEFAULT"
fetch notices: Illegal member variable name
bool(false)
unserialize notices: Illegal member variable name | unserialize(): Error at offset %d of %d bytes
bool(true)
bool(true)
bool(true)
undeclared notices: 0
