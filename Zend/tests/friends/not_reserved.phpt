--TEST--
Friends: `friend` is not reserved
--FILE--
<?php

namespace ClazzDemo {
	class Friend implements \InterfaceDemo\Friend {
		use \TraitDemo\Friend;

		public mixed $Friend;

		public function Friend(): Friend {
			var_dump($this->Friend);
			return $this;
		}
	}

	class Subclass extends Friend {}
}

namespace InterfaceDemo {
	interface Friend {}
}

namespace TraitDemo {
	trait Friend {}
}

namespace EnumDemo {
	enum Friend {
		case Friend;
	}
}

namespace {
	use ClazzDemo\Friend;
	use ClazzDemo\Subclass;

	$f = new Friend;
	$f->Friend = "demo";
	$f->Friend();

	$f = new Subclass;
	$f->Friend = "demo";
	$f->Friend();

	var_dump(\EnumDemo\Friend::Friend);
}

?>
--EXPECT--
string(4) "demo"
string(4) "demo"
enum(EnumDemo\Friend::Friend)
