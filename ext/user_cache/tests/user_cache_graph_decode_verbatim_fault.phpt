--TEST--
UserCache\Cache: a verbatim array that fails structural validation makes the fetch unrestorable while other payloads keep decoding
--SKIPIF--
<?php
if (!PHP_DEBUG) die('skip requires a debug build (fault injection is ZEND_DEBUG-only)');
?>
--ENV--
USER_CACHE_DEBUG_VERBATIM_ARR_INVALID=1
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

$cache = UserCache\Cache::getPool('graph-decode-verbatim-fault');
$cache->clear();

ok('stores', $cache->store('root', [1, 2, 3])
    && $cache->store('nested', ['o' => new stdClass(), 'leaf' => [1, 2]])
    && $cache->store('objects', ['o' => new stdClass(), 'n' => 7])
    && $cache->store('string', 'text'));

ok('verbatim root yields the default', $cache->fetch('root', 'DEFAULT') === 'DEFAULT');
ok('verbatim root is unrestorable', !$cache->has('root'));
ok('nested verbatim leaf yields the default', $cache->fetch('nested', 'DEFAULT') === 'DEFAULT');
ok('nested verbatim leaf is unrestorable', !$cache->has('nested'));
ok('no error is raised', error_get_last() === null);

$objects = $cache->fetch('objects');
ok('object graph still decodes', $objects['o'] instanceof stdClass && $objects['n'] === 7);
ok('string still decodes', $cache->fetch('string') === 'text');
$multiple = $cache->fetchMultiple(['root', 'objects', 'string'], 'DEFAULT');
ok('multiple', array_keys($multiple) === ['root', 'objects', 'string']
    && $multiple['root'] === 'DEFAULT'
    && $multiple['objects'] == $objects
    && $multiple['string'] === 'text');
?>
--EXPECT--
stores: OK
verbatim root yields the default: OK
verbatim root is unrestorable: OK
nested verbatim leaf yields the default: OK
nested verbatim leaf is unrestorable: OK
no error is raised: OK
object graph still decodes: OK
string still decodes: OK
multiple: OK
