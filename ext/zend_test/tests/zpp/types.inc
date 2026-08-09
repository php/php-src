<?php

class S {
	public function __toString(): string {
		return 'S class';
	}
}

return [
	null,
	false,
	true,
	42,
	73.5,
	'string',
	[],
	new stdClass(),
	new S(),
	STDOUT,
];
