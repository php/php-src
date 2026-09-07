--TEST--
session_create_id() when the create_sid handler throws
--INI--
session.save_handler=files
session.name=PHPSESSID
session.gc_probability=0
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

class MySessionHandler extends SessionHandler
{
    public int $calls = 0;

    public function create_sid(): string
    {
        if ($this->calls++ > 0) {
            throw new Exception('create_sid failed');
        }
        return parent::create_sid();
    }

    public function validateId(string $id): bool
    {
        return false;
    }
}

session_set_save_handler(new MySessionHandler(), true);
session_start();

try {
    session_create_id();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
    $previous = $e->getPrevious();
    echo $previous::class, ": ", $previous->getMessage(), PHP_EOL;
}

var_dump(session_status() === PHP_SESSION_ACTIVE);

?>
--EXPECT--
TypeError: Session id must be of type string, null given
Exception: create_sid failed
bool(true)
