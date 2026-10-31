--TEST--
GH-25108 (json_encode() skips uninitialized hooked properties)
--FILE--
<?php

class UninitializedProperties
{
    public int $plain;
    public int $hooked {
        set => $value;
    }
    public ?int $hookedNullable {
        set => $value;
    }
}

class MixedProperties
{
    public int $firstMissing {
        set => $value;
    }
    public string $first = 'first';
    public ?string $middleMissing {
        set => $value;
    }
    public ?string $getterNull {
        get => null;
    }
    public ?string $backedNull = null {
        set => $value;
    }
    public int $backedValue = 42 {
        set => $value;
    }
    public string $last = 'last';
    public int $lastMissing {
        set => $value;
    }
    public ?string $virtualSetOnly {
        set {}
    }
}

$empty = new UninitializedProperties();
echo json_encode($empty), "\n";

$empty->hooked = 7;
$empty->hookedNullable = null;
echo json_encode($empty), "\n";

$mixed = new MixedProperties();
echo json_encode($mixed), "\n";
echo json_encode($mixed, JSON_PRETTY_PRINT), "\n";

?>
--EXPECT--
{}
{"hooked":7,"hookedNullable":null}
{"first":"first","getterNull":null,"backedNull":null,"backedValue":42,"last":"last"}
{
    "first": "first",
    "getterNull": null,
    "backedNull": null,
    "backedValue": 42,
    "last": "last"
}
