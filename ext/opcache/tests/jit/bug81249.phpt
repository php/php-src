--TEST--
Bug #81249: Intermittent property assignment failure with JIT enabled
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php

declare(strict_types=1);

final class EucJpDecoder
{
    private const JIS0212_INDEX = [108 => 728];
    private const JIS0208_INDEX = [];

    private const CONTINUE = -1;
    private const FINISHED = -2;
    private const ERROR = -3;

    /**
     * @var int
     */
    private $lead;

    /**
     * @var bool
     */
    private $isJis0212;

    public function __construct()
    {
        $this->lead = 0x00;
        $this->isJis0212 = false;
    }

    public function handle(array &$ioQueue, string $byte): int
    {
        if ($byte === '') {
            if ($this->lead !== 0x00) {
                $this->lead = 0x00;

                return self::ERROR;
            }

            return self::FINISHED;
        }

        $byte = ord($byte);

        if ($this->lead === 0x8E && ($byte >= 0xA1 && $byte <= 0xDF)) {
            $this->lead = 0x00;

            return 0xFF61 - 0xA1 + $byte;
        }

        if ($this->lead === 0x8F && ($byte >= 0xA1 && $byte <= 0xFE)) {
            $this->isJis0212 = true;
            $this->lead = $byte;

            return self::CONTINUE;
        }

        if ($this->lead !== 0x00) {
            $lead = $this->lead;
            $this->lead = 0x00;
            $codePoint = null;

            if (($lead >= 0xA1 && $lead <= 0xFE) && ($byte >= 0xA1 && $byte <= 0xFE)) {
                $index = self::JIS0208_INDEX;

                if ($this->isJis0212) {
                    $index = self::JIS0212_INDEX;
                }

                $codePoint = $index[($lead - 0xA1) * 94 + $byte - 0xA1] ?? null;
            }

            $this->isJis0212 = false;

            if ($codePoint !== null) {
                return $codePoint;
            }

            if ($byte <= 0x7F) {
                array_unshift($ioQueue, chr($byte));
            }

            return self::ERROR;
        }

        if ($byte <= 0x7F) {
            return $byte;
        }

        if ($byte === 0x8E || $byte === 0x8F || ($byte >= 0xA1 && $byte <= 0xFE)) {
            $this->lead = $byte;

            return self::CONTINUE;
        }

        return self::ERROR;
    }
}

for ($i = 0; $i < 2000; ++$i) {
    $decoder = new EucJpDecoder();

    $bytes = ["\x8F", "\xA2", "\xAF", ''];
    $out = null;

    foreach ($bytes as $byte) {
        $result = $decoder->handle($bytes, $byte);

        if ($result >= 0) {
            $out = $result;
        }
    }

    // $bytes array should be decoded to U+2D8, decimal 728
    assert($out === 728);
}
?>
OK
--EXPECT--
OK
