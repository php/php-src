<?php

class Manager implements ContextManager
{
    public function __construct(private bool $rethrow = true)
    {
    }

    public function enterContext(): mixed
    {
        echo __METHOD__, "()\n";
        return new stdClass;
    }

    public function exitContext(?\Throwable $e = null): ?bool
    {
        echo __METHOD__, "(", ($e ? get_class($e) . '(' . $e->getMessage() . ')' : 'null'), ")\n";
        return !$this->rethrow;
    }
}

?>
