--TEST--
Dom\HTMLDocument::createFromFile() streaming UTF-8 fast-path handles multi-byte sequences across >=3 short reads
--EXTENSIONS--
dom
--FILE--
<?php

class MultiChunkUtf8Stream {
    public $context;
    public static array $cases = [];
    private array $chunks = [];
    private int $index = 0;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool {
        $key = parse_url($path, PHP_URL_HOST);
        $this->chunks = self::$cases[$key];
        $this->index = 0;
        return true;
    }

    public function stream_read(int $count): string {
        return $this->chunks[$this->index++] ?? '';
    }

    public function stream_eof(): bool {
        return $this->index >= count($this->chunks);
    }

    public function stream_stat(): array {
        return [];
    }
}

stream_wrapper_register('utf8chunk', MultiChunkUtf8Stream::class);

MultiChunkUtf8Stream::$cases = [
    'utf3'  => ['<p>', "\xE2", "\x82", "\xAC</p>"],          // U+20AC across 3 reads
    'utf4a' => ['<p>', "\xF0", "\x9F", "\x98\x80</p>"],      // U+1F600 across 3 reads
    'utf4b' => ['<p>', "\xF0", "\x9F", "\x98", "\x80</p>"],  // U+1F600 across 4 reads
    'trunc' => ['<p>', "\xF0", "\x9F"],                      // Truncated sequence at EOF -> U+FFFD
];

foreach (array_keys(MultiChunkUtf8Stream::$cases) as $case) {
    $dom = Dom\HTMLDocument::createFromFile("utf8chunk://$case", LIBXML_NOERROR);
    echo $case, ': ', bin2hex($dom->getElementsByTagName('p')[0]->textContent), "\n";
}
?>
--EXPECT--
utf3: e282ac
utf4a: f09f9880
utf4b: f09f9880
trunc: efbfbd
