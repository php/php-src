--TEST--
Benchmark scenario catalog exposes the vote-prep scenarios
--FILE--
<?php
require __DIR__ . '/../../../opcache_static_cache_benchmark/src/BenchmarkScenarioCatalog.php';

foreach (BenchmarkScenarioCatalog::names() as $name) {
	$scenario = BenchmarkScenarioCatalog::get($name);
	echo $name, "\t", $scenario['runner'], "\t", count($scenario['cases']), "\t", count($scenario['backends']);
	if (isset($scenario['concurrency'])) {
		echo "\t", $scenario['concurrency'], "\t", $scenario['key_mode'], "\t", $scenario['key_space'];
	}
	echo "\n";
}
?>
--EXPECT--
vote_read_long	read	7	9
carbon_datetime_compare	read	2	9
fetch_mutate_object	read	1	3
vote_write_throughput	write	5	3	1	distinct	32
vote_write_contention_shared	write	5	3	5	shared	1
vote_write_contention_distinct	write	5	3	5	distinct	16
vote_entry_reservation_contention	write	2	3	5	shared	1
