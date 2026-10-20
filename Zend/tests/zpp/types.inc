<?php

class S {
	public function __toString(): string {
		return 'S class';
	}
}

$anon = new class {};

return [
	'null' => null,
	'false' => false,
	'true' => true,
	'42' => 42,
	'73.5' => 73.5,
	"'string'" => 'string',
	"'15'" => '15',
	"'56.7'" => '56.7',
	"'stdClass'" => 'stdClass',
	'anon class name' => $anon::class,
	'[]' => [],
	'new stdClass()' => new stdClass(),
	'new S()' => new S(),
	'STDOUT' => STDOUT,
];
