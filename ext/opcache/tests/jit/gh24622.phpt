--TEST--
GH-24622: Tracing JIT must spill inherited register values before property helpers
--EXTENSIONS--
opcache
spl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
zend.assertions=1
assert.exception=1
--CREDITS--
frodeborli
--FILE--
<?php

final class StringBuffer
{
    private \SplDoublyLinkedList $queue;
    private string $buffer = '';
    private int $length = 0;
    private int $offset = 0;
    private int $totalRead = 0;
    private int $totalWritten = 0;
    private bool $ended = false;

    public function __construct()
    {
        $this->queue = new \SplDoublyLinkedList();
    }

    public function isEmpty(): bool
    {
        return $this->offset === $this->length && $this->queue->isEmpty();
    }

    public function write(string $chunk): void
    {
        if ($this->ended) {
            throw new \RuntimeException('Buffer has been ended');
        }
        $this->totalWritten += \strlen($chunk);
        $this->queue->push($chunk);
    }

    public function read(int $maxLength): string
    {
        if ($maxLength < 0) {
            throw new \OutOfBoundsException("Can't read negative lengths");
        }
        $this->fill($maxLength);
        $chunk = \substr($this->buffer, $this->offset, $maxLength);
        $length = \strlen($chunk);
        $this->offset += $length;
        $this->totalRead += $length;

        return $chunk;
    }

    public function end(): void
    {
        if ($this->ended) {
            throw new \LogicException('StringBuffer already ended');
        }
        $this->ended = true;
    }

    public function eof(): bool
    {
        return $this->ended && $this->offset === $this->length && $this->queue->isEmpty();
    }

    public function unread(string $chunk): void
    {
        if ($this->ended && $this->offset === $this->length && $this->queue->isEmpty()) {
            throw new \LogicException("Can't unread to an ended and empty StringBuffer");
        }
        $chunkLength = \strlen($chunk);
        $this->totalRead -= $chunkLength;
        if ($this->length === $this->offset) {
            $this->queue->unshift($chunk);
        } else {
            $this->buffer = $chunk . \substr($this->buffer, $this->offset);
            $this->length += $chunkLength - $this->offset;
            $this->offset = 0;
        }
    }

    protected function fill(int $requiredLength): bool
    {
        if ($this->offset > 4096) {
            $this->buffer = \substr($this->buffer, $this->offset);
            $this->length -= $this->offset;
            $this->offset = 0;
        }
        while ($this->length < $this->offset + $requiredLength && !$this->queue->isEmpty()) {
            $chunk       = $this->queue->shift();
            $chunkLength = \strlen($chunk);

            if ($this->length === $this->offset) {
                $this->buffer = $chunk;
                $this->length = $chunkLength;
                $this->offset = 0;
            } else {
                $this->buffer .= $chunk;
                $this->length += $chunkLength;
            }
        }

        return $this->length >= $this->offset + $requiredLength;
    }
}

$sb = new StringBuffer();
$count = 0;
for ($i = 0; $i < 1000; ++$i) { $sb->write('x'); }
$sb->end();
while (!$sb->eof()) { if ('' !== $sb->read(1)) { ++$count; } }
assert($count === 1000);

$sb = new StringBuffer();
$totalReceived = 0;
$chunk = str_repeat('x', 1024);
for ($i = 0; $i < 100; ++$i) { $sb->write($chunk); }
$sb->end();
while (!$sb->eof()) { $totalReceived += strlen($sb->read(8192)); }
assert($totalReceived === 102400);

$sb = new StringBuffer();
assert($sb->eof() === false);
$sb->write('Hello');
assert($sb->read(10000) === 'Hello');
assert($sb->read(100) === '');
assert($sb->write('ab') === null);
assert($sb->read(1) === 'a');
assert($sb->read(2) === 'b');
$sb->write('abc');
assert($sb->eof() === false);
$sb->write('def');
$sb->end();
$sb->unread('b');
assert($sb->read(5) === 'babcd');
var_dump($sb->eof());
assert($sb->read(10000) === 'ef');
var_dump($sb->eof());

--EXPECT--
bool(false)
bool(true)
