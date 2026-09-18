--TEST--
UserCache\Cache: column-block graph nodes round-trip every key and property shape
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
enum ColumnBlockTag: string {
	case A = 'a';
}

class ColumnBlockPlain {
	public $p0; public $p1; public $p2; public $p3; public $p4; public $p5; public $p6; public $p7;
	public $p8; public $p9; public $p10; public $p11; public $p12; public $p13; public $p14; public $p15;
	public $p16; public $p17; public $p18; public $p19; public $p20; public $p21; public $p22; public $p23;
	public $p24; public $p25; public $p26; public $p27; public $p28; public $p29; public $p30; public $p31;
	public $p32; public $p33; public $p34; public $p35; public $p36; public $p37; public $p38; public $p39;
}

class ColumnBlockSleep {
	public $kept = 'k';
	public $dropped = 'd';
	private $secret = 's';
	public $tag;

	public function __sleep(): array {
		return ['kept', 'secret', 'tag'];
	}
}

/* An enum member keeps every container off the verbatim path, so all of
 * these arrays are encoded as column blocks. */
$tag = ColumnBlockTag::A;

$packed = range(0, 99);
$packed[] = $tag;

$holes = [0 => 'a', 1 => 'b', 2 => 'c', 3 => $tag];
unset($holes[1]);

$sparse = [5 => 'five', -3 => 'minus', PHP_INT_MAX => 'max', 0 => $tag];

$strings = [];
for ($i = 0; $i < 40; $i++) {
	$strings["col$i"] = $i % 2 ? (float) $i : "v$i";
}
$strings['tag'] = $tag;

$mixed = ['x' => 1, 7 => 'seven', 'y' => [1, 2, 3], -1 => null, 'z' => $tag, 8 => true];

$plain = new ColumnBlockPlain();
for ($i = 0; $i < 40; $i++) {
	$plain->{"p$i"} = $i * 1.5;
}
$plain->p39 = $tag;

$sleep = new ColumnBlockSleep();
$sleep->tag = $tag;

$cache = UserCache\Cache::getPool('column-blocks');
$payload = compact('packed', 'holes', 'sparse', 'strings', 'mixed', 'plain', 'sleep');
var_dump($cache->store('payload', $payload));

$fetched = $cache->fetch('payload');

var_dump($fetched['packed'] === $packed, array_is_list($fetched['packed']));
var_dump($fetched['holes'] === $holes, array_keys($fetched['holes']));
/* PHP_INT_MAX differs per platform, so compare the keys rather than print them. */
var_dump($fetched['sparse'] === $sparse, array_keys($fetched['sparse']) === [5, -3, PHP_INT_MAX, 0]);
var_dump($fetched['strings'] === $strings);
var_dump($fetched['mixed'] === $mixed, array_keys($fetched['mixed']));
var_dump($fetched['plain'] == $plain, $fetched['plain']->p39 === $tag, $fetched['plain']->p17);
var_dump($fetched['sleep']->kept, $fetched['sleep']->dropped, $fetched['sleep']->tag === $tag);
var_dump((array) $fetched['sleep'] === (array) $sleep);

/* A packed value column costs 9 bytes per element, not a 32-byte node. */
$probe = UserCache\Cache::getPool('column-blocks-probe');
$probe->store('packed', [...range(0, 999), $tag]);
var_dump($probe->getPoolStatus()->getUsedMemory() < 1000 * 24);

/* Rows of up to 32 string keys use the shared shape encoding; 33 keys fall
 * back to per-element key columns. */
function rows(int $keys): array {
	$rows = [];
	for ($r = 0; $r < 50; $r++) {
		$row = [];
		for ($k = 0; $k < $keys; $k++) {
			$row["column$k"] = $r * 100 + $k;
		}
		$row['tag'] = ColumnBlockTag::A;
		$rows[] = $row;
	}

	return $rows;
}

$shaped = UserCache\Cache::getPool('column-blocks-shaped');
$dynamic = UserCache\Cache::getPool('column-blocks-dynamic');
$shaped->store('rows', rows(31));
$dynamic->store('rows', rows(32));
var_dump($shaped->fetch('rows') === rows(31), $dynamic->fetch('rows') === rows(32));
var_dump($shaped->getPoolStatus()->getUsedMemory() < $dynamic->getPoolStatus()->getUsedMemory());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(true)
bool(true)
bool(true)
bool(true)
array(6) {
  [0]=>
  string(1) "x"
  [1]=>
  int(7)
  [2]=>
  string(1) "y"
  [3]=>
  int(-1)
  [4]=>
  string(1) "z"
  [5]=>
  int(8)
}
bool(true)
bool(true)
float(25.5)
string(1) "k"
string(1) "d"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
