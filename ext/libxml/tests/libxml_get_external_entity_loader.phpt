--TEST--
libxml_get_external_entity_loader() returns current handler
--EXTENSIONS--
libxml
--FILE--
<?php

class Handler {
	private $name;

	public function __construct($name) {
		$this->name = $name;
	}

	public function handle($public, $system, $context) {
		return null;
	}

	public function __toString() {
		return "Handler#{$this->name}";
	}
}

var_dump(libxml_get_external_entity_loader());
libxml_set_external_entity_loader([new Handler('A'), 'handle']);
print libxml_get_external_entity_loader()[0] . "\n";
libxml_set_external_entity_loader([new Handler('B'), 'handle']);
print libxml_get_external_entity_loader()[0] . "\n";
libxml_set_external_entity_loader(null);
var_dump(libxml_get_external_entity_loader());

--EXPECT--
NULL
Handler#A
Handler#B
NULL
