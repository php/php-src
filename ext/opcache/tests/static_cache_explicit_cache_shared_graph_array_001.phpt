--TEST--
OPcache explicit cache stores nested array shared graphs without relocation-sensitive layout
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

class RelocatableArrayNode
{
	public function __construct(public array $metadata) {}
}

$payload = [
	'routes' => [
		['path' => '/users', 'methods' => ['GET', 'POST']],
		['path' => '/posts', 'defaults' => ['page' => 1, 'sort' => 'desc']],
	],
	'nodes' => [
		new RelocatableArrayNode(['flags' => ['auth' => true, 'cache' => false]]),
		new RelocatableArrayNode(['flags' => ['auth' => false, 'cache' => true]]),
	],
];

var_dump(OPcache\volatile_store('relocatable-array-payload', $payload));
$copy = OPcache\volatile_fetch('relocatable-array-payload');

var_dump($copy['routes'][0]['methods'][1]);
var_dump($copy['routes'][1]['defaults']['page']);
var_dump($copy['nodes'][0] instanceof RelocatableArrayNode);
var_dump($copy['nodes'][0]->metadata['flags']['auth']);
var_dump($copy['nodes'][1]->metadata['flags']['cache']);

OPcache\pinned_store('relocatable-array-object', new RelocatableArrayNode([
	'matrix' => [[1, 2], [3, 4]],
]));
$global = OPcache\pinned_fetch('relocatable-array-object');

var_dump($global instanceof RelocatableArrayNode);
var_dump($global->metadata['matrix'][1][0]);

var_dump(OPcache\volatile_store('relocatable-array-payload', [
	'routes' => [['path' => '/health', 'methods' => ['GET']]],
]));
$updated = OPcache\volatile_fetch('relocatable-array-payload');

var_dump($updated['routes'][0]['path']);
var_dump($updated['routes'][0]['methods'][0]);

?>
--EXPECT--
bool(true)
string(4) "POST"
int(1)
bool(true)
bool(true)
bool(true)
bool(true)
int(3)
bool(true)
string(7) "/health"
string(3) "GET"
