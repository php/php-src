--TEST--
GH-21341: suffix-as-keys walk uses offset-tracked snprintf (no strcat overflow)
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (getenv('SKIP_ASAN')) die('skip Timeouts under ASAN');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

/* Exercise the suffix-as-keys code path (SNMP_USE_SUFFIX_AS_KEYS |
 * SNMP_CMD_WALK).  Walking .1.3.6.1.2.1 (mib-2) returns entries whose
 * suffixes have multiple components (e.g. "1.1.0", "2.1.1.0"), driving
 * several iterations of the snprintf loop that GH-21341 replaced the
 * strcat loop with. */
$session = new SNMP(SNMP::VERSION_2c, $hostname, $community, $timeout, $retries);
$result = $session->walk('.1.3.6.1.2.1', /* suffix_as_keys */ true);

var_dump(is_array($result));
var_dump(count($result) > 0);

/* Every key must be a bare dotted-numeric suffix, not a full OID.
 * Full OIDs start with '.'; suffixes do not. */
$bad = array_filter(array_keys($result), static function (string $k): bool {
    return $k[0] === '.' || !preg_match('/^\d[\d.]*$/', $k);
});
var_dump(count($bad) === 0);

/* No key may end with '.': the trailing-dot removal (buf2[pos-1] = '\0')
 * must fire correctly on every entry. */
$trailing = array_filter(array_keys($result), static function (string $k): bool {
    return $k !== '' && $k[-1] === '.';
});
var_dump(count($trailing) === 0);

var_dump($session->close());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
