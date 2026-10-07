--TEST--
Fileinfo reconstruction UAF with re-entrant file info constructor (stream stat)
--EXTENSIONS--
fileinfo
--CREDITS--
Calif.io
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php

final class AuditFileinfoStream
{
    public $context;
    private int $offset = 0;
    private static bool $reentered = false;

    public function stream_open(string $path, string $mode, int $options, ?string &$openedPath): bool
    {
        if (!self::$reentered) {
            self::$reentered = true;
            echo "stream-open-reconstruct\n";
            $GLOBALS['finfo']->__construct(FILEINFO_MIME_TYPE);
        }
        return true;
    }

    public function stream_stat(): false
    {
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

stream_wrapper_register('auditfinfo', AuditFileinfoStream::class);

$finfo = new finfo(FILEINFO_MIME_TYPE);
var_dump($finfo->file('auditfinfo://attacker-controlled-upload'));

?>
--EXPECTF--
stream-open-reconstruct

Warning: finfo::file(): AuditFileinfoStream::stream_cast is not implemented! in %s on line %d
string(15) "application/pdf"
