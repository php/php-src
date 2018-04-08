--TEST--
Bug #74442: Opcached version produces a nested array
--CREDITS--
Eric Norris <erictnorris@gmail.com>
--FILE--
<?php
class Schema_Base {
    public function addField($typeclass, array $params = null) {
        $field = new $typeclass($params);
        return $field;
    }
}

class Field_Base {
    public function __construct(array $params = null) {
        if (! is_array($params)) {
            $params = (array) $params;
        }
        call_user_func_array(array($this, 'acceptParams'), $params);
    }
}

class Field_Integer extends Field_Base {
    protected function acceptParams($bytes = 4) {
        echo print_r($bytes, true);
    }
}

try {
    $schema = new Schema_Base;
    $schema->addField('Field_Integer');
} catch (Throwable $ex) {
    echo "CAUGHT EXCEPTION";
    echo (string)$ex;
}

?>
--EXPECT--
4
