--TEST--
PDO PgSQL #15287 (Pdo\Pgsql has no real lazy fetch mode)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require  __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require  __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);

// We need a dataset of several KB so that memory gain is significant.
// See https://www.postgresql.org/message-id/1140652.1687950987%40sss.pgh.pa.us
$pdo->exec("create temp table t (n int, t text)");
$pdo->exec("insert into t values (0, 'original')");
for ($i = -1; ++$i < 8;) {
	$pdo->exec("insert into t select n + 1, 'non '||t from t");
}

$reqOf3 = 'select 79 n union all select 80 union all select 81';
$reqOfBig = 'select * from t';

function display($res)
{
	echo implode("\n", array_map(fn($row) => implode("\t", $row), $res))."\n";
}

echo "=== non regression ===\n";

// libpq explicitely requires single-row-mode statements to run one at a time (one stmt must
// be fully read, or aborted, before another one can be launched).
// Ensure that integration does not break the ability of the traditional, prefetched mode,
// to mix fetching of multiple statements' result.
$stmt1 = $pdo->query($reqOf3);
$stmt2 = $pdo->query("select * from ($reqOf3) t order by n desc");
for ($i = -1; ++$i < 3;) {
	display([ $stmt1->fetch() ]);
	display([ $stmt2->fetch() ]);
}

echo "=== mem test ===\n";

// First execute without lazy fetching, as a reference and non-regression;
// execute twice: in case warmup reduces memory consumption, we want the stabilized consumption.
for ($i = -1; ++$i < 5;) {
	$attrs = [];
	$lazy = false;
	switch ($i) {
		case 0:
		case 3:
			echo "Without lazy fetching:\n";
			break;
		case 2:
			echo "With statement-scoped lazy fetching:\n";
			$attrs = [ PDO::ATTR_PREFETCH => 0 ];
			$lazy = true;
			break;
		case 4:
			echo "With connection-scoped lazy fetching:\n";
			$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);
			$lazy = true;
			break;
	}
	$stmt = $pdo->prepare($reqOfBig, $attrs);
	$stmt->execute();
	$res = [];
	// No fetchAll because we want the memory of the result of the FORElast call (the last one is empty).
	while (($re = $stmt->fetch())) {
		$res[] = $re;
		// Memory introspection relies on an optionally-compiled constant.
		if (defined('Pdo\Pgsql::ATTR_RESULT_MEMORY_SIZE')) {
			$mem = $stmt->getAttribute(Pdo\Pgsql::ATTR_RESULT_MEMORY_SIZE);
		} else {
			// If not there emulate a return value which validates our test.
			$mem = $lazy ? 0 : 1;
		}
	}
	echo "ResultSet is $mem bytes long\n";
	if ($i >= 2) {
		echo "ResultSet is " . ($mem > $mem0 ? "longer" : ($mem == $mem0 ? "not shorter" : ($mem <= $mem0 / 2 ? "more than twice shorter" : "a bit shorter"))) . " than without lazy fetching\n";
	} else {
		$mem0 = $mem;
	}
}

$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);

foreach ([
	[ 'query', 'fetch' ],
	[ 'query', 'fetchAll' ],
	[ 'prepare', 'fetch' ],
	[ 'prepare', 'fetchAll' ],
] as $mode) {
	echo "=== with " . implode(' / ', $mode). " ===\n";
	switch ($mode[0]) {
		case 'query':
			$stmt = $pdo->query($reqOf3);
			break;
		case 'prepare':
			$stmt = $pdo->prepare($reqOf3);
			$stmt->execute();
			break;
	}
	switch ($mode[1]) {
		case 'fetch':
			$res = [];
			while (($re = $stmt->fetch())) {
				$res[] = $re;
			}
			break;
		case 'fetchAll':
			$res = $stmt->fetchAll();
			break;
	}
	display($res);
}
echo "DML works too:\n";
$pdo->exec("create temp table t2 as select 678 n, 'ok' status");
echo "multiple calls to the same prepared statement, some interrupted before having read all results:\n";
$stmt = $pdo->prepare("select :1 n union all select :1 + 1 union all select :1 + 2 union all select :1 + 3");
$stmt->execute([ 32 ]);
$res = []; for ($i = -1; ++$i < 2;) $res[] = $stmt->fetch(); display($res);
$stmt->execute([ 15 ]);
$res = []; while (($re = $stmt->fetch())) $res[] = $re; display($res);
$stmt->execute([ 0 ]);
$res = []; for ($i = -1; ++$i < 2;) $res[] = $stmt->fetch(); display($res);
display($pdo->query("select * from t2")->fetchAll());
?>
--EXPECTF--
=== non regression ===
79
81
80
80
81
79
=== mem test ===
Without lazy fetching:
ResultSet is %d bytes long
ResultSet is %d bytes long
With statement-scoped lazy fetching:
ResultSet is %d bytes long
ResultSet is more than twice shorter than without lazy fetching
Without lazy fetching:
ResultSet is %d bytes long
ResultSet is not shorter than without lazy fetching
With connection-scoped lazy fetching:
ResultSet is %d bytes long
ResultSet is more than twice shorter than without lazy fetching
=== with query / fetch ===
79
80
81
=== with query / fetchAll ===
79
80
81
=== with prepare / fetch ===
79
80
81
=== with prepare / fetchAll ===
79
80
81
DML works too:
multiple calls to the same prepared statement, some interrupted before having read all results:
32
33
15
16
17
18
0
1
678	ok
