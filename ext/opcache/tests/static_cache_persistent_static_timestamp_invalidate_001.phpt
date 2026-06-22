--TEST--
OPcache StableStatic state is discarded when a cached class definition changes
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php

$subject = __DIR__ . '/stable_static_timestamp_invalidate_001_subject.php';

$writeSubject = static function (string $version) use ($subject): void {
	file_put_contents($subject, <<<PHP
<?php
#[OPcache\\StableStatic]
class TimestampInvalidationState
{
	public static int \$value = 0;
}

echo '{$version}:', ++TimestampInvalidationState::\$value, "\\n";
PHP);
	clearstatcache(true, $subject);
};

$writeSubject('v1');

$php = getenv('TEST_PHP_EXECUTABLE');
if ($php) {
	$php = realpath(__DIR__ . '/../../../' . $php) ?: $php;
	putenv('TEST_PHP_EXECUTABLE=' . $php);
}

include 'php_cli_server.inc';
php_cli_server_start('-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.static_cache.stable_size_mb=32 -d opcache.validate_timestamps=1 -d opcache.revalidate_freq=0 -d opcache.file_update_protection=0');

$url = 'http://' . PHP_CLI_SERVER_ADDRESS . '/stable_static_timestamp_invalidate_001_subject.php';
echo file_get_contents($url);
echo file_get_contents($url);

sleep(1);
$writeSubject('v2');
echo file_get_contents($url);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stable_static_timestamp_invalidate_001_subject.php');
?>
--EXPECT--
v1:1
v1:2
v2:1
