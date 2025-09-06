--TEST--
Test DateTimeInterface::isWeekend() and DateTimeInterface::isWeekday() : inheritance functionality
--FILE--
<?php
// Set the default time zone
date_default_timezone_set("UTC");

echo "*** Testing DateTimeInterface::isWeekend() and DateTimeInterface::isWeekday() with inheritance ***\n";

// Create custom class that extends DateTime
class MyDateTime extends DateTime
{
    // No additional implementation needed - should inherit methods
}

// Create custom class that extends DateTime
class MyCustomDateTime extends DateTime
{
    // Override the methods to provide custom implementations
    
    #[\ReturnTypeWillChange]
    public function isWeekend()
    {
        // Custom implementation that returns the same result
        $day = (int)$this->format('w');
        return ($day === 0 || $day === 6);
    }

    #[\ReturnTypeWillChange]
    public function isWeekday()
    {
        // Custom implementation that returns the same result
        $day = (int)$this->format('w');
        return ($day >= 1 && $day <= 5);
    }
}

// Test with inherited class
echo "\nTesting MyDateTime (extends DateTime):\n";
$date1 = new MyDateTime("2025-09-06"); // Saturday
echo "Saturday 2025-09-06 is weekend: ";
var_dump($date1->isWeekend());
echo "Saturday 2025-09-06 is weekday: ";
var_dump($date1->isWeekday());

$date2 = new MyDateTime("2025-09-08"); // Monday
echo "Monday 2025-09-08 is weekend: ";
var_dump($date2->isWeekend());
echo "Monday 2025-09-08 is weekday: ";
var_dump($date2->isWeekday());

// Test with custom class extending DateTime with overridden methods
echo "\nTesting MyCustomDateTime (extends DateTime with overrides):\n";
$date3 = new MyCustomDateTime("2025-09-06"); // Saturday
echo "Saturday 2025-09-06 is weekend: ";
var_dump($date3->isWeekend());
echo "Saturday 2025-09-06 is weekday: ";
var_dump($date3->isWeekday());

$date4 = new MyCustomDateTime("2025-09-08"); // Monday
echo "Monday 2025-09-08 is weekend: ";
var_dump($date4->isWeekend());
echo "Monday 2025-09-08 is weekday: ";
var_dump($date4->isWeekday());

?>
--EXPECT--
*** Testing DateTimeInterface::isWeekend() and DateTimeInterface::isWeekday() with inheritance ***

Testing MyDateTime (extends DateTime):
Saturday 2025-09-06 is weekend: bool(true)
Saturday 2025-09-06 is weekday: bool(false)
Monday 2025-09-08 is weekend: bool(false)
Monday 2025-09-08 is weekday: bool(true)

Testing MyCustomDateTime (extends DateTime with overrides):
Saturday 2025-09-06 is weekend: bool(true)
Saturday 2025-09-06 is weekday: bool(false)
Monday 2025-09-08 is weekend: bool(false)
Monday 2025-09-08 is weekday: bool(true)
