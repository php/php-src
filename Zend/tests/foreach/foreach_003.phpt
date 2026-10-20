--TEST--
Iterator exceptions in foreach by value
--FILE--
<?php
class IT implements Iterator {
    private $n = 0;
    private $count = 0;
    private $trap = null;

    function __construct($count, $trap = null) {
        $this->count = $count;
        $this->trap = $trap;
    }

    function trap($trap) {
        if ($trap === $this->trap) {
            throw new Exception($trap);
        }
    }

    function rewind(): void  {$this->trap(__FUNCTION__); $this->n = 0;}
    function valid(): bool   {$this->trap(__FUNCTION__); return $this->n < $this->count;}
    function key(): mixed     {$this->trap(__FUNCTION__); return $this->n;}
    function current(): mixed {$this->trap(__FUNCTION__); return $this->n;}
    function next(): void    {$this->trap(__FUNCTION__); $this->n++;}
}

foreach(['rewind', 'valid', 'key', 'current', 'next'] as $trap) {
    $obj = new IT(3, $trap);
    try {
        // IS_CV
        foreach ($obj as $key => $val) echo "$val\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    unset($obj);

    try {
        // IS_VAR
        foreach (new IT(3, $trap) as $key => $val) echo "$val\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
        // IS_TMP_VAR
        foreach ((object)new IT(2, $trap) as $key => $val) echo "$val\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Exception: rewind
Exception: rewind
Exception: rewind
Exception: valid
Exception: valid
Exception: valid
Exception: key
Exception: key
Exception: key
Exception: current
Exception: current
Exception: current
0
Exception: next
0
Exception: next
0
Exception: next
