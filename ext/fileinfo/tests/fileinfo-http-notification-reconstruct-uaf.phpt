--TEST--
Fileinfo reconstruction UAF with re-entrant file info constructor (HTTP notification)
--EXTENSIONS--
fileinfo
--CREDITS--
Calif.io
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
	echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php

include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$reentered = false;
$spray = [];
$finfo = new finfo(FILEINFO_MIME_TYPE);

$context = stream_context_create([], [
    'notification' => static function (
        int $notificationCode,
        int $severity,
        ?string $message,
        int $messageCode,
        int $bytesTransferred,
        int $bytesMax,
    ) use (&$reentered, &$spray, $finfo): void {
        if ($reentered) {
            return;
        }
        $reentered = true;
        echo "http-notification-reconstruct:$notificationCode\n";
        try {
            $finfo->__construct(
                FILEINFO_MIME_TYPE,
                dirname(__DIR__) . '/fixtures/does-not-exist.magic',
            );
        } catch (Throwable $error) {
            echo "constructor-failed\n";
        }
        for ($i = 0; $i < 128; ++$i) {
            $spray[] = str_repeat('A', 279);
        }
        echo "same-bin-spray-complete\n";
    },
]);

php_cli_server_start();
$url = "http://" . PHP_CLI_SERVER_ADDRESS;
var_dump($finfo->file($url, 0, $context));

?>
--EXPECTF--
http-notification-reconstruct:2
constructor-failed
same-bin-spray-complete

Warning: finfo::file(): Failed identify data 22:Magic database is not open in %s on line %d

Fatal error: Uncaught Error: Invalid finfo object in %s:%d
Stack trace:
#0 %s(%d): finfo->file('http://localhos...', 0, Resource id #%d)
#1 {main}
  thrown in %s on line %d
