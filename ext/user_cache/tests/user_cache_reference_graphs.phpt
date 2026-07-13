--TEST--
UserCache\Cache: references and cyclic graphs survive fetch and access
--EXTENSIONS--
user_cache
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
$cache = UserCache\Cache::getPool('reference-graphs');

class UCReferenceHolder
{
	public int $n = 0;
}

class UCReferencePair
{
	public $a;
	public $b;
}

class UCTypedReferencePair
{
	public int $a;
	public int $b;
}

class UCCyclePayload
{
	public ?UCCyclePayload $peer = null;

	public function __construct(
		public string $name,
		public int $revision,
	) {
	}

	public function describe(): string
	{
		return $this->name . ':' . $this->revision . ':' . $this->peer?->name;
	}
}

class UCReferencedPayload
{
	public function __construct(
		public string $label,
		public int $revision,
	) {
	}
}

class UCReferenceAssignmentPayload
{
	public UCReferencedPayload $alias;

	public function __construct(
		public string $name,
		public UCReferencedPayload $child,
	) {
		$this->alias = $child;
	}
}

function uc_build_cycle_payload(string $rootName, string $peerName, int $revision): UCCyclePayload
{
	$root = new UCCyclePayload($rootName, $revision);
	$peer = new UCCyclePayload($peerName, $revision + 1);
	$root->peer = $peer;
	$peer->peer = $root;

	return $root;
}

$a = ['value' => 1, 'tag' => 't'];
$a['alias'] = &$a['value'];
var_dump($cache->store('k_scalar', $a));

$g = $cache->fetch('k_scalar');
$g['value'] = 99;
echo "scalar alias follows write: ";
var_dump($g['alias'] === 99);
echo "scalar other fetch is independent: ";
var_dump($cache->fetch('k_scalar')['value'] === 1);

$x = 10;
$b = ['r' => &$x, 'tag' => 'u'];
var_dump($cache->store('k_single', $b));
echo "single value: ";
var_dump($cache->fetch('k_single')['r'] === 10);

$o = new UCReferenceHolder();
$o->n = 7;
$c = [];
$c['p'] = &$o;
$c['q'] = &$o;
var_dump($cache->store('k_objref', $c));

$gc = $cache->fetch('k_objref');
echo "objref identity: ";
var_dump($gc['p'] === $gc['q']);
$gc['p']->n = 123;
echo "objref shared mutation: ";
var_dump($gc['q']->n === 123);

$pair = new UCReferencePair();
$v = 1;
$pair->a = &$v;
$pair->b = &$v;
var_dump($cache->store('k_proptref', $pair));
echo "proptref round-trips to object: ";
var_dump($cache->fetch('k_proptref') instanceof UCReferencePair);

$gp = $cache->fetch('k_proptref');
$gp->a = 55;
echo "proptref alias preserved: ";
var_dump($gp->b === 55);

$typed = new UCTypedReferencePair();
$typedValue = 11;
$typed->a = &$typedValue;
$typed->b = &$typedValue;
var_dump($cache->store('k_typed_proptref', $typed));
$gt = $cache->fetch('k_typed_proptref');
$gt->a = 77;
echo "typed proptref alias preserved: ";
var_dump($gt->b === 77);
try {
	$gt->a = 'not an int';
} catch (TypeError $e) {
	echo "typed proptref type check: TypeError\n";
}

$dynamic = new stdClass();
$dynamicValue = 1;
$dynamic->a = &$dynamicValue;
$dynamic->b = &$dynamicValue;
var_dump($cache->store('dpr_scalar', $dynamic));
$gd = $cache->fetch('dpr_scalar');
$gd->a = 99;
echo "dynamic scalar alias: ";
var_dump($gd->b === 99);
echo "dynamic independent fetch: ";
var_dump($cache->fetch('dpr_scalar')->a === 1);

$shared = (object) ['n' => 0];
$dynamicObject = new stdClass();
$dynamicObject->x = &$shared;
$dynamicObject->y = &$shared;
var_dump($cache->store('dpr_obj', $dynamicObject));
$gdo = $cache->fetch('dpr_obj');
echo "dynamic object identity: ";
var_dump($gdo->x === $gdo->y);
$gdo->x->n = 7;
echo "dynamic object shared mutation: ";
var_dump($gdo->y->n === 7);

$self = ['n' => 5, 'list' => [1, 2, 3]];
$self['self'] = &$self;
var_dump($cache->store('k_self', $self));
$gs = $cache->fetch('k_self');
echo "self cycle resolves: ";
var_dump($gs['self'] === $gs);
echo "value through cycle: ", $gs['self']['n'], " ", $gs['self']['list'][1], "\n";

$p = ['name' => 'p'];
$q = ['name' => 'q'];
$p['other'] = &$q;
$q['other'] = &$p;
var_dump($cache->store('k_mutual', $p));
$gm = $cache->fetch('k_mutual');
echo "mutual cycle resolves: ";
var_dump($gm['other']['other'] === $gm);
echo "names: ", $gm['name'], " ", $gm['other']['name'], "\n";

$root = ['a' => ['b' => ['c' => 7]]];
$root['a']['b']['back'] = &$root;
var_dump($cache->store('k_deep', $root));
$deep = $cache->fetch('k_deep');
echo "deep cycle resolves: ";
var_dump($deep['a']['b']['back'] === $deep);
echo "deep value: ", $deep['a']['b']['back']['a']['b']['c'], "\n";

$g1 = $cache->fetch('k_self');
$g2 = $cache->fetch('k_self');
$g1['n'] = 99;
echo "fetches independent: ";
var_dump($g2['n'] === 5);

$serializedCycle = uc_build_cycle_payload('serialized-root', 'serialized-peer', 1);
var_dump($cache->store('serialized_cycle_object', $serializedCycle));
$fetchedSerializedCycle = $cache->fetch('serialized_cycle_object');
echo "serialized cycle access: ", $fetchedSerializedCycle->describe(), "\n";
echo "serialized cycle back edge: ";
var_dump($fetchedSerializedCycle->peer->peer === $fetchedSerializedCycle);

$referenceAssignment = new UCReferenceAssignmentPayload(
	'reference-root',
	new UCReferencedPayload('reference-child', 1),
);
var_dump($cache->store('reference_assignment_object', $referenceAssignment));
$fetchedReferenceAssignment = $cache->fetch('reference_assignment_object');
echo "reference assignment access: ";
var_dump($fetchedReferenceAssignment->name . ':' . $fetchedReferenceAssignment->child->label . ':' . $fetchedReferenceAssignment->child->revision);
echo "reference assignment identity: ";
var_dump($fetchedReferenceAssignment->child === $fetchedReferenceAssignment->alias);
$fetchedReferenceAssignment->child->revision = 9;
echo "reference assignment shared mutation: ";
var_dump($fetchedReferenceAssignment->alias->revision === 9);

$cycleAssignment = uc_build_cycle_payload('cycle-root', 'cycle-peer', 1);
var_dump($cache->store('cycle_assignment_object', $cycleAssignment));
$fetchedCycleAssignment = $cache->fetch('cycle_assignment_object');
echo "cycle assignment access: ", $fetchedCycleAssignment->describe(), "\n";
echo "cycle assignment back edge: ";
var_dump($fetchedCycleAssignment->peer->peer === $fetchedCycleAssignment);

unset(
	$a, $g, $x, $b, $o, $c, $gc, $pair, $v, $gp,
	$typed, $typedValue, $gt, $dynamic, $dynamicValue, $gd,
	$shared, $dynamicObject, $gdo, $self, $gs, $p, $q, $gm,
	$root, $deep, $g1, $g2, $serializedCycle, $fetchedSerializedCycle,
	$referenceAssignment, $fetchedReferenceAssignment, $cycleAssignment,
	$fetchedCycleAssignment
);
gc_collect_cycles();
?>
--EXPECT--
bool(true)
scalar alias follows write: bool(true)
scalar other fetch is independent: bool(true)
bool(true)
single value: bool(true)
bool(true)
objref identity: bool(true)
objref shared mutation: bool(true)
bool(true)
proptref round-trips to object: bool(true)
proptref alias preserved: bool(true)
bool(true)
typed proptref alias preserved: bool(true)
typed proptref type check: TypeError
bool(true)
dynamic scalar alias: bool(true)
dynamic independent fetch: bool(true)
bool(true)
dynamic object identity: bool(true)
dynamic object shared mutation: bool(true)
bool(true)
self cycle resolves: bool(true)
value through cycle: 5 2
bool(true)
mutual cycle resolves: bool(true)
names: p q
bool(true)
deep cycle resolves: bool(true)
deep value: 7
fetches independent: bool(true)
bool(true)
serialized cycle access: serialized-root:1:serialized-peer
serialized cycle back edge: bool(true)
bool(true)
reference assignment access: string(32) "reference-root:reference-child:1"
reference assignment identity: bool(true)
reference assignment shared mutation: bool(true)
bool(true)
cycle assignment access: cycle-root:1:cycle-peer
cycle assignment back edge: bool(true)
