<?php

class b extends a {
	public function test() {
		echo __METHOD__ . "()\n";
		parent::test();
	}
}
