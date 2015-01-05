Example 1: A singleton (static members)
=======================================

<?php

	class Counter {
		var $counter = 0;
		
		function increment_and_print()
		{
				print ++$this->counter;
				print "\n";
		}	
	}
	
	
	class SingletonCounter {
		static $m_instance = NULL;
		
		function Instance()
		{
				if (self::$m_instance == NULL) {
					self::$m_instance = new Counter();
				}
				return self::$m_instance;
		}	
	}
	
	SingletonCounter::Instance()->increment_and_print();
	SingletonCounter::Instance()->increment_and_print();
	SingletonCounter::Instance()->increment_and_print();
	
?>

Example 2: Factory method (derefencing objects returned from functions)
=======================================================================

<?php
	
	class Circle {
		function draw()
		{
			print "Circle\n";	
		}
	}
	
	class Square {
		function draw()
		{
			print "Square\n";	
		}
	}
	
	function ShapeFactoryMethod($shape)
	{
		switch ($shape) {
			case "Circle":
				return new Circle();
			case "Square":
				return new Square();
		}	
	}
	
	ShapeFactoryMethod("Circle")->draw();
	ShapeFactoryMethod("Square")->draw();
	
	
?>

Example 3: Class constants and class scope
==========================================

<?php

	class ErrorCodes {
		const FATAL = "Fatal error\n";
		const WARNING = "Warning\n";
		const INFO = "Informational message\n";
		
		function print_fatal_error_codes()
		{
			print "FATAL = " . FATAL;
			print "self::FATAL = " . self::FATAL;
		}
	}
	
	/* Call the static function and move into the ErrorCodes scope */
	ErrorCodes::print_fatal_error_codes();

?>

Example 4: Regular object method using both local and global functions
======================================================================

<?php

	class HelloWorld {
		const HELLO_WORLD = "Hello, World!\n";
		
		function get_hello_world()
		{
			return HELLO_WORLD;
		}
		
		function length_of_hello_world()
		{
			$str = $this->get_hello_world();
			return strlen($str);
		}	
	}
	
	$obj = new HelloWorld();
	print "length_of_hello_world() = " . $obj->length_of_hello_world();
	print "\n";
?>

Example 5: Multiple derefencing of objects returned from methods
================================================================

<?php


	class Name {
		function Name($_name)
		{
			$this->name = $_name;	
		}
		
		function display()
		{
			print $this->name;
			print "\n";
		}
	}
	
	class Person {
		function Person($_name, $_address)
		{
			$this->name = new Name($_name);
		}
		
		function getName()
		{
			return $this->name;	
		}
	}	

	$person = new Person("John", "New York");
	$person->getName()->display();
	
?>

Example 6: Exception handling
=============================

<?
	class MyException {
		function MyException($_error) {
			$this->error = $_error;	
		}
		
		function getException()
		{
			return $this->error;	
		}
	}
	
	function ThrowException()
	{
		throw new MyException("'This is an exception!'");	
	}
	
	
	try {
	} catch (MyException $exception) {
		print "There was an exception: " . $exception->getException();
		print "\n";
	}
	
	try {
		ThrowException();	
	} catch (MyException $exception) {
		print "There was an exception: " . $exception->getException();
		print "\n";
	}
	
?>

Example 7: __clone()
===================

<?
	class MyCloneable {
		static $id = 0;
		
		function MyCloneable()
		{
			$this->id = self::$id++;
		}
		
		function __clone()
		{
			$this->name = $that->name;
			$this->address = "New York";
			$this->id = self::$id++;
		}
	}
	

	
	$obj = new MyCloneable();

	$obj->name = "Hello";
	$obj->address = "Tel-Aviv";
	
	print $obj->id;
	print "\n";
	
	$obj = $obj->__clone();
	
	print $obj->id;
	print "\n";
	print $obj->name;
	print "\n";
	print $obj->address;
	print "\n";
?>

Example 8: Unified constructors
===============================

<?

	class BaseClass {
		function __construct()
		{
			print "In BaseClass constructor\n";	
		}
	}
	
	class SubClass extends BaseClass {
		function __construct()
		{
				parent::__construct();
				print "In SubClass constructor\n";	
		}
	}
	
	$obj = new BaseClass();
	
	$obj = new SubClass();
	
?>

Example 9: Destructors
=======================

<?php

class MyDestructableClass {
	function __construct()
	{
		print "In constructor\n";
		$this->name = "MyDestructableClass";
	}	
	
	function __destruct()
	{
		print "Destroying " . $this->name . "\n";
	}	
}

$obj = new MyDestructableClass();

?>
