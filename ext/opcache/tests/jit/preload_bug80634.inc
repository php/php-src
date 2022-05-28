<?php
class SomeClass extends \DatePeriod {
	public ?DateTimeInterface $current;

	public function __construct(int $v) {
		parent::__construct(new \DateTime('2020-12-31'), new \DateInterval("P1Y"), 1);
		$this->current = $v;
		var_dump($this->current);
	}
}
