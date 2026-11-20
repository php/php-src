--TEST--
Test Csv\LazyLaxCollection::createFromFile(): default dialect delivers a row after a single partial read
--EXTENSIONS--
csv
--FILE--
<?php
/* With the default dialect no lookahead past a complete row is needed, so the first row
 * must be delivered after one read, without triggering another (potentially blocking)
 * read first — the behaviour a FIFO with a slow producer depends on. */
class CountingStream {
    public static int $reads = 0;
    public $context;
    private array $chunks = ["a,b\r\n", "c,d\r\n"];
    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool { return true; }
    public function stream_read(int $count): string {
        self::$reads++;
        return array_shift($this->chunks) ?? '';
    }
    public function stream_eof(): bool { return $this->chunks === []; }
    public function stream_close(): void {}
    public function stream_stat(): array|false { return false; }
}
stream_wrapper_register('csvcount', CountingStream::class);

$rows = [];
foreach (Csv\LazyLaxCollection::createFromFile('csvcount://input') as $row) {
    $rows[] = [CountingStream::$reads, $row];
}
foreach ($rows as [$readsSoFar, $row]) {
    echo $readsSoFar, ': ', json_encode($row), \PHP_EOL;
}
?>
--EXPECT--
1: ["a","b"]
2: ["c","d"]
