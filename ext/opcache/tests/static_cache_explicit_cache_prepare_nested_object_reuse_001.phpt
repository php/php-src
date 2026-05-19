--TEST--
OPcache explicit cache tracks nested userland object mutations across repeated stores
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
opcache.static_cache.pinned_size_mb=32
--FILE--
<?php

final class PreparedNestedLeaf
{
	public function __construct(
		public string $label,
		public int $revision,
	) {}
}

$payload = [
	'name' => 'first',
	'leaf' => new PreparedNestedLeaf('leaf-first', 1),
	'rows' => [['state' => 'alpha']],
];

var_dump(OPcache\volatile_store('nested_volatile_first', $payload));
OPcache\pinned_store('nested_pinned_first', $payload);

$payload['name'] = 'second';
$payload['leaf']->label = 'leaf-second';
$payload['leaf']->revision = 2;
$payload['rows'][0]['state'] = 'beta';

var_dump(OPcache\volatile_store('nested_volatile_second', $payload));
OPcache\pinned_store('nested_pinned_second', $payload);

$volatileFirst = OPcache\volatile_fetch('nested_volatile_first');
$volatileSecond = OPcache\volatile_fetch('nested_volatile_second');
$pinnedFirst = OPcache\pinned_fetch('nested_pinned_first');
$pinnedSecond = OPcache\pinned_fetch('nested_pinned_second');

echo $volatileFirst['name'], "\n";
echo $volatileFirst['leaf']->label, "\n";
echo $volatileFirst['leaf']->revision, "\n";
echo $volatileFirst['rows'][0]['state'], "\n";
echo $volatileSecond['name'], "\n";
echo $volatileSecond['leaf']->label, "\n";
echo $volatileSecond['leaf']->revision, "\n";
echo $volatileSecond['rows'][0]['state'], "\n";
echo $pinnedFirst['name'], "\n";
echo $pinnedFirst['leaf']->label, "\n";
echo $pinnedFirst['leaf']->revision, "\n";
echo $pinnedFirst['rows'][0]['state'], "\n";
echo $pinnedSecond['name'], "\n";
echo $pinnedSecond['leaf']->label, "\n";
echo $pinnedSecond['leaf']->revision, "\n";
echo $pinnedSecond['rows'][0]['state'], "\n";

?>
--EXPECT--
bool(true)
bool(true)
first
leaf-first
1
alpha
second
leaf-second
2
beta
first
leaf-first
1
alpha
second
leaf-second
2
beta
