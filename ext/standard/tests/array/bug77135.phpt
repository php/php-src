--TEST--
Test extract() with $this
--FILE--
<?php

class Extract
{
    public function run(): void
    {
        $options = [
            'EXTR_OVERWRITE' => EXTR_OVERWRITE,
            'EXTR_SKIP' => EXTR_SKIP,
            'EXTR_PREFIX_SAME' => EXTR_PREFIX_SAME,
            'EXTR_PREFIX_ALL' => EXTR_PREFIX_ALL,
            'EXTR_PREFIX_INVALID' => EXTR_PREFIX_INVALID,
            'EXTR_IF_EXISTS' => EXTR_IF_EXISTS,
            'EXTR_PREFIX_IF_EXISTS' => EXTR_PREFIX_IF_EXISTS,
        ];

        foreach ($options as $name => $flags) {
            echo "{$name}\n";

            $this->handle($name, $flags);
            $this->handle("{$name}_REFS", $flags | EXTR_REFS);
            echo "\n";
        }
    }

    private function handle(string $name, int $flags): void
    {
        $array = ["this" => "value"];

        try {
            $result = extract($array, $flags, "x");
            echo "  extract() = {$result}\n";

            echo "  \$this = " . get_class($this) . "\n";
            echo "  \$v_this = " . (isset($x_this) ? $x_this : "NULL") . "\n";
        } catch (\Throwable $e) {
            echo "  Exception: " . $e->getMessage() . "\n";
        }
    }
}

(new Extract)->run();

?>
--EXPECT--
EXTR_OVERWRITE
  Exception: Cannot re-assign $this
  Exception: Cannot re-assign $this

EXTR_SKIP
  extract() = 0
  $this = Extract
  $v_this = NULL
  extract() = 0
  $this = Extract
  $v_this = NULL

EXTR_PREFIX_SAME
  extract() = 1
  $this = Extract
  $v_this = value
  extract() = 1
  $this = Extract
  $v_this = value

EXTR_PREFIX_ALL
  extract() = 1
  $this = Extract
  $v_this = value
  extract() = 1
  $this = Extract
  $v_this = value

EXTR_PREFIX_INVALID
  extract() = 1
  $this = Extract
  $v_this = value
  extract() = 1
  $this = Extract
  $v_this = value

EXTR_IF_EXISTS
  extract() = 0
  $this = Extract
  $v_this = NULL
  extract() = 0
  $this = Extract
  $v_this = NULL

EXTR_PREFIX_IF_EXISTS
  extract() = 0
  $this = Extract
  $v_this = NULL
  extract() = 0
  $this = Extract
  $v_this = NULL
