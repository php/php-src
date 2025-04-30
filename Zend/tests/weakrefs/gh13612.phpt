--TEST--
GH-13612 (Corrupted memory in destructor with weak references)
--FILE--
<?php

class WeakAnalysingMapRepro
{
    public array $destroyed = [];
    public array $ownerDestructorHandlers = [];

    public function __construct()
    {
        $handler = new class($this) {
            private \WeakReference $weakAnalysingMap;

            public function __construct(WeakAnalysingMapRepro $analysingMap)
            {
                $this->weakAnalysingMap = \WeakReference::create($analysingMap);
            }

            public function __destruct()
            {
                var_dump($this->weakAnalysingMap->get());
            }
        };

        $this->destroyed[] = 1;
        $this->ownerDestructorHandlers[] = $handler;
    }
}

new WeakAnalysingMapRepro();

echo "Done\n";

?>
--EXPECT--
NULL
Done
