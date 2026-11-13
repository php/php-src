--TEST--
Request boundary releases a header callback registered between two requests
--EXTENSIONS--
zend_test
--FILE--
<?php
final class Tracker
{
    /** @var list<string> */
    public static array $log = [];

    public function __destruct()
    {
        self::$log[] = 'destroyed';
    }
}

zend_test_request_boundary(static function (): void {
    $tracker = new Tracker();
    header_register_callback(static function () use ($tracker): void {
        Tracker::$log[] = 'called';
    });
});

Tracker::$log[] = 'next request';
echo implode("\n", Tracker::$log), "\n";
?>
--EXPECT--
destroyed
next request
