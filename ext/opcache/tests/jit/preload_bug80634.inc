<?php
class SomeClass extends \DatePeriod {
	public $interval;

	public function __construct(int $v) {
		parent::__construct(new \DateTime('2020-12-31'), new \DateInterval("P1Y"), 1);
		$this->interval = $v;
		var_dump($this->interval);
	}
}