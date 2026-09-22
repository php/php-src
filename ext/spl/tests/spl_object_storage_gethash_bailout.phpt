--TEST--
SplObjectStorage getHash() depth counter is reset after a bailout in a user getHash()
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    die("skip sapi/cli/tests/php_cli_server.inc required but not found");
}
?>
--INI--
allow_url_fopen=1
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$code = <<<'PHP'
if ($_SERVER["REQUEST_URI"] === "/poison") {
    class Poison extends SplObjectStorage {
        public function getHash($o): string {
            ini_set("memory_limit", "2M");
            str_repeat("a", 100 * 1024 * 1024);
            return "x";
        }
    }
    (new Poison())->offsetSet(new stdClass());
    echo "poison";
} else {
    $s = new SplObjectStorage();
    $s->offsetSet(new stdClass());
    echo "check-ok count=", count($s);
}
PHP;

php_cli_server_start($code, 'router.php');

$base = 'http://' . PHP_CLI_SERVER_ADDRESS;
// Request 1 bails out (OOM) inside the overridden getHash() mid-offsetSet.
@file_get_contents($base . '/poison');
// A later request on the same worker must not be poisoned by a stuck counter.
echo @file_get_contents($base . '/check'), "\n";
echo @file_get_contents($base . '/check'), "\n";
?>
--EXPECT--
check-ok count=1
check-ok count=1
