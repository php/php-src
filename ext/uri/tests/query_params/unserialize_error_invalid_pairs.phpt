--TEST--
Test Uri\QueryParams unserialize() - error - malformed pairs and unsupported stored types
--FILE--
<?php

function unserializeQueryParams(array $data): void {
    try {
        unserialize('O:15:"Uri\QueryParams"' . substr(serialize($data), 1));
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

$data = (new Uri\QueryParams())->__serialize();

unserializeQueryParams([["params" => ["a=1"]], $data[1]]);
unserializeQueryParams([["params" => [["a"]]], $data[1]]);
unserializeQueryParams([["params" => [["a", "1", "extra"]]], $data[1]]);
unserializeQueryParams([["params" => [[1 => "a", 2 => "1"]]], $data[1]]);
unserializeQueryParams([["params" => [[1, "value"]]], $data[1]]);
unserializeQueryParams([["params" => [["a", true]]], $data[1]]);
unserializeQueryParams([["params" => [["a", []]]], $data[1]]);
unserializeQueryParams([["params" => [["a", new stdClass()]]], $data[1]]);
unserializeQueryParams([["params" => [["valid", "value"], ["invalid", 1]]], $data[1]]);

?>
--EXPECT--
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
