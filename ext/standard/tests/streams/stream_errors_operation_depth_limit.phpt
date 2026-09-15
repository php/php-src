--TEST--
Stream errors: operations refused by the depth limit keep the stack consistent
--INI--
zend.max_allowed_stack_size=-1
--FILE--
<?php
class RecursiveStream
{
    public $context;
    public static int $depth = 0;
    private int $reads = 0;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        $outermost = self::$depth === 0;
        if (++self::$depth < 1010) {
            $f = fopen('recursive-stream://x', 'r', false, $GLOBALS['ctx']);
            fread($f, 1);
            fclose($f);
        }
        self::$depth--;
        if (!$outermost) {
            return 'x';
        }
        return ++$this->reads <= 2 ? str_repeat('A', $count + 1) : '';
    }

    public function stream_eof(): bool
    {
        return self::$depth > 0 || $this->reads >= 3;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

stream_wrapper_register('recursive-stream', RecursiveStream::class);

$depthWarnings = 0;
set_error_handler(static function (int $severity, string $message) use (&$depthWarnings): bool {
    if (str_contains($message, 'depth exceeded')) {
        $depthWarnings++;
        return true;
    }
    return false;
});

$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_store' => StreamErrorStore::All,
    'error_handler' => static function (array $errors): void {
        echo "handler: " . count($errors) . " error(s)\n";
    },
]]);

$stream = fopen('recursive-stream://x', 'r', false, $ctx);
var_dump(strlen(stream_get_contents($stream)));
fclose($stream);
var_dump($depthWarnings > 0, count(stream_last_errors()));
?>
--EXPECT--
handler: 2 error(s)
int(16384)
bool(true)
int(2)
