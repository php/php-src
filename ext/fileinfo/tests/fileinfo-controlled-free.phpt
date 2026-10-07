--TEST--
Controlled free with re-entrant file info constructor
--EXTENSIONS--
fileinfo
--CREDITS--
Calif.io
--FILE--
<?php

final class AuditFileinfoControlledStream
{
    public $context;
    private int $offset = 0;
    private static bool $reentered = false;

    public function stream_open(string $path, string $mode, int $options, ?string &$openedPath): bool
    {
        if (!self::$reentered) {
            self::$reentered = true;
            echo "stream-open-release\n";
            try {
                $GLOBALS['finfo']->__construct(
                    FILEINFO_MIME_TYPE,
                    dirname(__DIR__) . '/fixtures/does-not-exist.magic',
                );
            } catch (Throwable $error) {
                echo "constructor-failed\n";
            }

            for ($i = 0; $i < 128; ++$i) {
                $GLOBALS['spray'][] = str_repeat('A', 279);
            }
            echo "same-bin-spray-complete\n";
        }
        return true;
    }

    public function stream_stat(): false
    {
        return false;
    }

    public function url_stat(string $path, int $flags): array|false {
        return false;
    }

    public function stream_read(int $count): string
    {
        $data = "%PDF-1.7\n1 0 obj\n<<>>\nendobj\n";
        $chunk = substr($data, $this->offset, $count);
        $this->offset += strlen($chunk);
        return $chunk;
    }

    public function stream_eof(): bool
    {
        return $this->offset >= 32;
    }
}

stream_wrapper_register('auditcontrolled', AuditFileinfoControlledStream::class);

$spray = [];
$finfo = new finfo(FILEINFO_MIME_TYPE);
var_dump($finfo->file('auditcontrolled://attacker-controlled-upload'));

?>
--EXPECTF--
stream-open-release
constructor-failed
same-bin-spray-complete

Warning: finfo::file(): Failed identify data 22:Magic database is not open in %s on line %d

Fatal error: Uncaught Error: Invalid finfo object in %s:%d
Stack trace:
#0 %s(%d): finfo->file('auditcontrolled...')
#1 {main}
  thrown in %s on line %d
