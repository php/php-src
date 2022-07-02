--TEST--
resource enchant_broker_request_pwl_dict(resource $broker, string $filename); function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if(!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
$pathPwlDict = __DIR__ . "/enchant_broker_request_pwl_dict.pwl";

if (is_object($broker)) {
    echo("OK\n");
    $requestDict = enchant_broker_request_pwl_dict($broker, $pathPwlDict);

    if (is_object($requestDict)) {
        echo("OK\n");
        $dictdescribe = enchant_dict_describe($requestDict);

        if ($pathPwlDict === $dictdescribe['file']) {
            echo("OK\n");
        } else {
            echo("broker dict describe is not a resource failed\n");
        }
    } else {
        echo("dict broker request pwl has failed\n");
    }
} else {
    echo("broker is not a resource; failed;\n");
}
echo "OK\n";
?>
--EXPECT--
OK
OK
OK
OK
