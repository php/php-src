--TEST--
UserCache\Cache: array keys, property and class names are interned once per segment
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_fork')) {
	die('skip requires pcntl');
}
?>
--INI--
user_cache.enable=1
user_cache.enable_cli=1
opcache.file_cache_only=0
user_cache.shm_size=16M
--FILE--
<?php
class InternedRow {
	public $alpha = 1;
	public $beta = 2;
	public $gamma = 3;
}

function row(int $i, int $keys = 40): array {
	$row = [];
	for ($k = 0; $k < $keys; $k++) {
		$row["column_$k"] = $i * 1000 + $k;
	}

	return $row;
}

/* Fetches pin their payloads for the rest of the request, which would keep
 * every interned key alive here; reads therefore run in short-lived child
 * processes and report through their exit status. */
function in_child(callable $check): bool {
	$pid = pcntl_fork();
	if ($pid === 0) {
		exit($check() ? 0 : 1);
	}

	pcntl_waitpid($pid, $status);

	return pcntl_wexitstatus($status) === 0;
}

$interned = static fn (): int => UserCache\Cache::getStatus()->getInternedKeyCount();
$rows = UserCache\Cache::getPool('intern-rows');
$objs = UserCache\Cache::getPool('intern-objects');

var_dump($interned());

/* The first row interns its 40 keys; later rows reference them. */
var_dump($rows->store('r0', row(0)));
var_dump($interned());
$first = $rows->getPoolStatus()->getUsedMemory();

for ($i = 1; $i <= 100; $i++) {
	$rows->store("r$i", row($i));
}
var_dump($interned());
$all = $rows->getPoolStatus()->getUsedMemory();
$perRow = ($all - $first) / 100;
/* A row that shares interned keys is far smaller than the first, which
 * still carries its own copies (40 zend_strings of 24+ bytes). */
var_dump($perRow < $first - 40 * 24);

var_dump(in_child(static function (): bool {
	$rows = UserCache\Cache::getPool('intern-rows');
	$fetched = $rows->fetch('r57');
	$keys = array_keys($fetched);

	return $fetched === row(57) &&
		$keys === array_keys(row(57)) &&
		$fetched['column_39'] === 57039 &&
		isset($fetched[$keys[3]]) &&
		strlen($keys[3]) === 8;
}));

/* Keys longer than the intern limit stay in their payload. */
$long = [str_repeat('k', 65) => 1, str_repeat('j', 64) => 2];
var_dump($rows->store('long', $long));
var_dump($interned());
var_dump(in_child(static fn (): bool => UserCache\Cache::getPool('intern-rows')->fetch('long') === $long));

/* Class and property names of objects are interned too. */
$count = $interned();
var_dump($objs->store('o1', new InternedRow()));
var_dump($interned() - $count);
$objs->store('o2', new InternedRow());
var_dump($interned() - $count);
var_dump(in_child(static fn (): bool => UserCache\Cache::getPool('intern-objects')->fetch('o2') == new InternedRow()));

/* Another process resolves the same interns, and its payload is readable
 * elsewhere through the shared string addresses. */
var_dump(in_child(static function (): bool {
	return UserCache\Cache::getPool('intern-rows')->store('child', row(7));
}));
var_dump($interned());
var_dump(in_child(static fn (): bool => UserCache\Cache::getPool('intern-rows')->fetch('child') === row(7)));

/* Deleting a pool sweeps the keys nothing references any more. */
var_dump(UserCache\Cache::deletePool('intern-rows'));
var_dump($interned());
var_dump(in_child(static fn (): bool => UserCache\Cache::getPool('intern-objects')->fetch('o1') == new InternedRow()));
var_dump(UserCache\Cache::deletePool('intern-objects'));
var_dump($interned());
?>
--EXPECT--
int(0)
bool(true)
int(40)
int(40)
bool(true)
bool(true)
bool(true)
int(41)
bool(true)
bool(true)
int(4)
int(4)
bool(true)
bool(true)
int(45)
bool(true)
bool(true)
int(4)
bool(true)
bool(true)
int(0)
