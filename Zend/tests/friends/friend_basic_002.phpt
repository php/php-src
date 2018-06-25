--TEST--
A class can declare multiple classes as friend
--FILE--
<?php

class PartyHost
{
	friend PartyGoer, Bouncer;

	private $name;

	public function __construct($name)
	{
		$this->name = $name;
	}
}

class PartyGoer
{
	public function greet(PartyHost $person)
	{
		echo "Your name was {$person->name}, right? Nice party!" . PHP_EOL;
	}
}

class Bouncer
{
	public function greet(PartyHost $person)
	{
		echo "Hey {$person->name}! If you need backup, let me know." . PHP_EOL;
	}
}

$host = new PartyHost('Dustin');
$attendee = new PartyGoer();
$bouncer = new Bouncer();

$attendee->greet($host);
$bouncer->greet($host);

?>
--EXPECT--
Your name was Dustin, right? Nice party!
Hey Dustin! If you need backup, let me know.