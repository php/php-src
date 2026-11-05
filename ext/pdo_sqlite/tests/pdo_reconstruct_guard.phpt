--TEST--
PDO::__construct() rejects reconstruction, reentrant construction and unsafe retry
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$persistent = [PDO::ATTR_PERSISTENT => true];

// Sequential reconstruction of a constructed handle (plain and persistent).
$p = new PDO('sqlite::memory:');
try {
    $p->__construct('sqlite::memory:');
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$q = new PDO('sqlite::memory:', null, null, $persistent);
try {
    $q->__construct('sqlite::memory:', null, null, $persistent);
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Reentrant construction: a uri: DSN opens a stream whose userland wrapper
// calls __construct() again on the same, still-constructing object.
class ReentWrapper
{
    public $context;
    private int $pos = 0;
    private string $data = "sqlite::memory:";
    public function stream_open($path, $mode, $options, &$opened): bool
    {
        global $obj;
        try {
            $obj->__construct('sqlite::memory:', null, null, [PDO::ATTR_PERSISTENT => true]);
        } catch (\Error $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        return true;
    }
    public function stream_read($count): string
    {
        $chunk = substr($this->data, $this->pos, $count);
        $this->pos += strlen($chunk);
        return $chunk;
    }
    public function stream_eof(): bool
    {
        return $this->pos >= strlen($this->data);
    }
    public function stream_stat()
    {
        return [];
    }
}
stream_wrapper_register('reent', ReentWrapper::class);
$obj = (new ReflectionClass(PDO::class))->newInstanceWithoutConstructor();
$obj->__construct('uri:reent://x', null, null, $persistent);

// Retry after a failed persistent construct (which already swapped in the
// persistent handle before the driver factory failed).
$r = (new ReflectionClass(PDO::class))->newInstanceWithoutConstructor();
try {
    $r->__construct('sqlite:/pdo-reconstruct-nonexistent/x.db', null, null, $persistent);
} catch (\Throwable $e) {
}
try {
    $r->__construct('sqlite::memory:', null, null, $persistent);
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// Destructor reentry while a failed persistent connect() is unwinding.
class ReconstructOnDestruct extends Pdo\Sqlite
{
    public function __destruct()
    {
        try {
            $this->__construct('sqlite::memory:', null, null, [PDO::ATTR_PERSISTENT => true]);
        } catch (\Error $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }
}
try {
    ReconstructOnDestruct::connect('sqlite:/pdo-reconstruct-nonexistent/x.db', null, null, $persistent);
} catch (\Throwable $e) {
}

echo "done\n";
?>
--EXPECT--
Error: PDO object is already constructed
Error: PDO object is already constructed
Error: PDO object is already constructed
Error: PDO object is already constructed
Error: ReconstructOnDestruct object is already constructed
done
