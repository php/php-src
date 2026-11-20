--TEST--
Test Uri\QueryParams unserialize() - error - invalid outer structure
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

unserializeQueryParams([]);
unserializeQueryParams([$data[0], $data[1], []]);
unserializeQueryParams([1 => $data[0], 2 => $data[1]]);
unserializeQueryParams([null, $data[1]]);
unserializeQueryParams([$data[0], null]);
unserializeQueryParams([$data[0], ["extra" => "value"]]);
unserializeQueryParams([[], $data[1]]);
unserializeQueryParams([["query" => "a=1"], $data[1]]);
unserializeQueryParams([["params" => [], "extra" => []], $data[1]]);
unserializeQueryParams([["params" => "a=1"], $data[1]]);
unserializeQueryParams([["params" => [1 => ["a", "1"]]], $data[1]]);

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
Exception: Invalid serialization data for Uri\QueryParams object
Exception: Invalid serialization data for Uri\QueryParams object
