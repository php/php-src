--TEST--
Bug #63305 (zend_mm_heap corrupted with traits)
--FILE--
<?php
new Attachment("");

function __autoload($class) {
    switch ($class) {
    case "Attachment":
        eval(<<<'PHP'
class Attachment extends File {
}
PHP
    );
        break;
    case "File":
        eval(<<<'PHP'
class File {
    use TDatabaseObject {
        TDatabaseObject::__construct as private databaseObjectConstruct;
    }
    public function __construct() {
    }
}
PHP
    );
        break;
    case "TDatabaseObject":
        eval(<<<'PHP'
trait TDatabaseObject {
    public function __construct() {
    }
}
PHP
    );
        break;
    }
    return TRUE;
}
echo "okey";
?>
--EXPECT--
okey
