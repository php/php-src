<?php
require_once (dirname(__FILE__).'/snmp_include.inc');

function get_snmp_test_agent_unavailable_reason(): ?string
{
    global $hostname, $community, $timeout;

    $probe = static fn(): ?string =>
        @snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout) === false
            ? 'NO SNMPD on this host or community invalid'
            : null;

    $directory = getenv('TEST_PHP_SHARED_CACHE_DIR');
    if (!is_string($directory) || !is_dir($directory)) {
        return $probe();
    }

    $configuration = [$hostname, $community, $timeout];
    $cacheFile = $directory
        . DIRECTORY_SEPARATOR
        . 'snmp-'
        . hash('sha256', serialize($configuration));
    $cache = @fopen($cacheFile, 'c+');
    if ($cache === false || !flock($cache, LOCK_EX)) {
        if (is_resource($cache)) {
            fclose($cache);
        }
        return $probe();
    }

    $cached = stream_get_contents($cache);
    $reason = $cached !== '' ? $cached : null;
    if ($reason === null) {
        // Only failures are shared; successful checks still probe the configured agent.
        $reason = $probe();
        if (is_string($reason)) {
            rewind($cache);
            ftruncate($cache, 0);
            fwrite($cache, $reason);
            fflush($cache);
        }
    }

    flock($cache, LOCK_UN);
    fclose($cache);
    return $reason;
}

//test server is available
// this require snmpget to work ...
//snmpget  ( string $hostname  , string $community  ,
//string $object_id  [, int $timeout  [, int $retries  ]] )

$reason = get_snmp_test_agent_unavailable_reason();
if (is_string($reason)) {
    die("skip $reason");
}
