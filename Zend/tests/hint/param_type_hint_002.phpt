--TEST--
Parameter type hint - Reflection
--FILE--
<?php

function test(int $a, double $b, string $c, boolean $d = true, resource $e, array $f = array(), object $g) {
}

$func = new ReflectionFunction('test');
Reflection::Export($func);

foreach ($func->getParameters() as $i => $param) {
    printf(
        "-- Parameter #%d: %s {\n".
        "   Allows NULL: %s\n".
        "   Hint Type: %s\n".
        "   Passed to by reference: %s\n".
        "   Is optional?: %s\n",
        $i, 
        $param->getName(),
        var_export($param->getClass(), 1),
        $param->getTypeHint(),
        var_export($param->allowsNull(), 1),
        var_export($param->isPassedByReference(), 1),        
        $param->isOptional() ? 'yes' : 'no'
    );
    printf("   Int: %d | Double: %d | Bool: %d | String: %d | Resource: %d | Array: %d | Object: %d\n", 
    	$param->isInt(), $param->isDouble(), $param->isBool(), $param->isString(),
    	$param->isResource(), $param->isArray(), $param->isObject());
		print "}\n";
}

?>
--EXPECTF--
Function [ <user> function test ] {
  @@ %s 3 - 4

  - Parameters [7] {
    Parameter #0 [ <required> integer $a ]
    Parameter #1 [ <required> double $b ]
    Parameter #2 [ <required> string $c ]
    Parameter #3 [ <required> boolean $d ]
    Parameter #4 [ <required> resource $e ]
    Parameter #5 [ <required> array $f ]
    Parameter #6 [ <required> object $g ]
  }
}

-- Parameter #0: a {
   Allows NULL: NULL
   Hint Type: integer
   Passed to by reference: false
   Is optional?: false
   Int: 1 | Double: 0 | Bool: 0 | String: 0 | Resource: 0 | Array: 0 | Object: 0
}
-- Parameter #1: b {
   Allows NULL: NULL
   Hint Type: double
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 1 | Bool: 0 | String: 0 | Resource: 0 | Array: 0 | Object: 0
}
-- Parameter #2: c {
   Allows NULL: NULL
   Hint Type: string
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 0 | Bool: 0 | String: 1 | Resource: 0 | Array: 0 | Object: 0
}
-- Parameter #3: d {
   Allows NULL: NULL
   Hint Type: boolean
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 0 | Bool: 1 | String: 0 | Resource: 0 | Array: 0 | Object: 0
}
-- Parameter #4: e {
   Allows NULL: NULL
   Hint Type: resource
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 0 | Bool: 0 | String: 0 | Resource: 1 | Array: 0 | Object: 0
}
-- Parameter #5: f {
   Allows NULL: NULL
   Hint Type: array
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 0 | Bool: 0 | String: 0 | Resource: 0 | Array: 1 | Object: 0
}
-- Parameter #6: g {
   Allows NULL: NULL
   Hint Type: object
   Passed to by reference: false
   Is optional?: false
   Int: 0 | Double: 0 | Bool: 0 | String: 0 | Resource: 0 | Array: 0 | Object: 1
}
